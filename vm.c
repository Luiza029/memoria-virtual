#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include <string.h>

int validar_enderecos(FILE *arq, int alg_pagina, int alg_tlb){
    printf("alg_pagina = %d\n", alg_pagina);
    printf("alg_tlb = %d\n", alg_tlb);
    char linha[256];
    int count = 0, numLinha = 0;
    char *pedaco, *fim;
    long valor;

    while(fgets(linha, sizeof(linha), arq) != NULL){
        numLinha++;

        pedaco = strtok(linha, " \t\r\n");
        if(pedaco == NULL){
            continue;
        }    

        valor = strtol(pedaco, &fim, 10);

        if(fim == pedaco || *fim != '\0' || valor < 0 || valor > MAX){
            printf("Erro: addresses.txt tem numero invalido na linha %d\n", numLinha);
            return -1;
        }

        if(strtok(NULL, " \t\r\n") != NULL){
            printf("Erro: Mais de um argumento na linha %d\n", numLinha);
            return -1;
        }

        count++;
    }

    if(count == 0){
        printf("Erro: nenhuma linha foi lida no arquivo\n");
        return -1;
    }
    
    rewind(arq);

    int pagina, offset, tlb_ocupado = 0, frame, proximo_frame = 0, page_fault = 0, fisico, dado, fifo = 0, pagina_antiga, relogio = 0, menor, escolhido;
    int tlb_paginas[TAM_TLB], tlb_frame[TAM_TLB], tabela_pagina[NUM_PAGINAS], etiqueta[NUM_FRAMES], horas[NUM_FRAMES];
    signed char memoria[TAM_PAGINA * NUM_FRAMES];

    for(int i=0; i<NUM_PAGINAS; i++){
        tabela_pagina[i] = -1;
    }

    FILE *disco = fopen("BACKING_STORE.bin", "rb");

    if(disco == NULL){
        printf("Erro: arquivo nao encontrado\n");
        return -1;
    }

    while(fgets(linha, sizeof(linha), arq) != NULL){
        int posicao_tlb = -1;

        pedaco = strtok(linha, " \t\r\n");
        if(pedaco == NULL){
            continue;
        }

        valor = strtol(pedaco, NULL, 10);

        pagina = valor / 256;
        offset = valor % 256;

        for(int i=0; i<tlb_ocupado; i++){
            if(tlb_paginas[i] == pagina){
                posicao_tlb = i;
                frame = tlb_frame[i];
                break;
            }
        }

        if(posicao_tlb == -1){
            frame = tabela_pagina[pagina];

            if(frame == -1){
                page_fault++;
                
                if(proximo_frame < NUM_FRAMES){
                    frame = proximo_frame;
                    proximo_frame++;
                }

                else{
                    if(alg_pagina == FIFO){
                        frame = fifo;
                        fifo = (fifo + 1) % NUM_FRAMES;
                    }

                    else{
                        menor = horas[0];
                        escolhido = 0;

                        for(int i=0; i<NUM_FRAMES; i++){
                            if(horas[i] < menor){
                                menor = horas[i];
                                escolhido = i;
                            }
                        }

                        frame = escolhido;
                    }

                    pagina_antiga = etiqueta[frame];
                    tabela_pagina[pagina_antiga] = -1;
                }

                tabela_pagina[pagina] = frame;
                etiqueta[frame] = pagina;

                fseek(disco, pagina * 256, SEEK_SET);
                fread(&memoria[frame * 256], 1, 256, disco);
            }
        }

        relogio++;
        fisico = frame * 256 + offset;
        horas[frame] = relogio;
        dado = memoria[fisico];

        printf("Virtual address: %ld Physical address: %d Value: %d\n", valor, fisico, dado);
    }

    printf("Page Faluts = %d\n", page_fault);
    fclose(disco);
    return count;
}