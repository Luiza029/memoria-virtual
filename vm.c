#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include <string.h>
#include <pthread.h>

int validar_enderecos(FILE *arq, int alg_pagina, int alg_tlb){
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

    int pagina, offset, tlb_ocupado = 0, frame, proximo_frame = 0, page_fault = 0, 
    fisico, fifo = 0, pagina_antiga, relogio = 0, menor, escolhido, linhaTLB, 
    fifoTLB = 0, tlb_hits = 0, dado;

    int tlb_paginas[TAM_TLB], tlb_frame[TAM_TLB], tabela_pagina[NUM_PAGINAS], etiqueta[NUM_FRAMES], horas[NUM_FRAMES], horasTLB[TAM_TLB];
    signed char memoria[TAM_PAGINA * NUM_FRAMES];

    for(int i=0; i<NUM_PAGINAS; i++){
        tabela_pagina[i] = -1;
    }

    FILE *disco = fopen("BACKING_STORE.bin", "rb");

    if(disco == NULL){
        printf("Erro: arquivo BACKING_STORE.bin nao encontrado\n");
        return -1;
    }

    for(int i=0; i<TAM_TLB; i++){
        tlb_paginas[i] = -1;
    }

    FILE *saida = fopen("correct.txt", "w");
    if(saida == NULL){
        printf("Erro: arquivo correct.txt nao encontrado\n");
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

        /*
        for(int i=0; i<tlb_ocupado; i++){
            if(tlb_paginas[i] == pagina){
                posicao_tlb = i;
                frame = tlb_frame[i];
                tlb_hits++;
                relogio++;
                horasTLB[i] = relogio;
                break;
            }
        }
        */

        pthread_t threads[TAM_TLB];
        TLB bilhetes[TAM_TLB];

        for(int i=0; i<TAM_TLB; i++){
            bilhetes[i].pagina = pagina;
            bilhetes[i].linha = i;
            bilhetes[i].achou = 0;
            bilhetes[i].tlbPaginas = tlb_paginas;
            pthread_create(&threads[i], NULL, ajudante, &bilhetes[i]);
        }

        for(int i=0; i<TAM_TLB; i++){
            pthread_join(threads[i], NULL);
        }

        for(int i=0; i<TAM_TLB; i++){
            if(bilhetes[i].achou == 1){
                posicao_tlb = i;
                frame = tlb_frame[i];
                tlb_hits++;
                horasTLB[i] = relogio;
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

                    for(int i=0; i<TAM_TLB; i++){
                        if(tlb_paginas[i] == pagina_antiga){
                            tlb_paginas[i] = -1;
                        }
                    }
                }

                tabela_pagina[pagina] = frame;
                etiqueta[frame] = pagina;

                fseek(disco, pagina * 256, SEEK_SET);
                fread(&memoria[frame * 256], 1, 256, disco);
            }

            if(tlb_ocupado < TAM_TLB){
                linhaTLB = tlb_ocupado;
                tlb_ocupado++;
            }

            else{
                if(alg_tlb == FIFO){
                    linhaTLB = fifoTLB;
                    fifoTLB = (fifoTLB + 1) % TAM_TLB;
                }

                else{
                    menor = horasTLB[0];
                    escolhido = 0;

                    for(int i=0; i<TAM_TLB; i++){
                        if(horasTLB[i] < menor){
                            menor = horasTLB[i];
                            escolhido = i;
                        }
                    }

                    linhaTLB = escolhido;
                }
            }

            tlb_paginas[linhaTLB] = pagina;
            tlb_frame[linhaTLB] = frame;
            horasTLB[linhaTLB] = relogio;
            posicao_tlb = linhaTLB;
        }

        relogio++;
        fisico = frame * 256 + offset;
        horas[frame] = relogio;
        dado = memoria[fisico];

        fprintf(saida, "Virtual address: %ld TLB: %d Physical address: %d Value: %d\n", valor, posicao_tlb, fisico, dado);
    }

    fprintf(saida,"Number of Translated Addresses = %d\n", count);

    fprintf(saida, "Page Faults = %d\n", page_fault);
    fprintf(saida, "Page Fault Rate = %.3f\n", (double) page_fault / count);

    fprintf(saida, "TLB Hits = %d\n", tlb_hits);
    fprintf(saida, "TLB Hit Rate = %.3f\n", (double) tlb_hits / count);

    fclose(disco);
    return count;
}

void *ajudante(void *arg){
    TLB *bilhete = (TLB*) arg;

    if(bilhete->tlbPaginas[bilhete->linha] == bilhete->pagina){
        bilhete->achou = 1;
    }

    return NULL;
}