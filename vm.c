#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include <string.h>

int validar_enderecos(FILE *arq){
    char linha[256];
    int count = 0, numLinha = 0;

    while(fgets(linha, sizeof(linha), arq) != NULL){
        numLinha++;

        char *pedaco = strtok(linha, " \t\r\n");
        if(pedaco == NULL){
            continue;
        }    

        char *fim;
        long valor = strtol(pedaco, &fim, 10);

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
    
    return count;
}