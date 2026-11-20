#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

int main(int argc, char *argv[]) {
    if(argc != 4){
        printf("Erro: Use ./vm <arquivo> <fifo|lru> <fifo|lru>\n");
        return 1;
    }

    int alg_pagina, alg_tlb;
    
    if(strcmp(argv[2], "fifo") != 0 && strcmp(argv[2], "lru") != 0){
        printf("Erro: algoritmo invalido\n");
        return 1;
    }
    
    if(strcmp(argv[2], "fifo") == 0){
        alg_tlb = FIFO;
    } else{
        alg_tlb = LRU;
    }

    if(strcmp(argv[3], "fifo") != 0 && strcmp(argv[3], "lru") != 0){
        printf("Erro: algoritmo invalido\n");
        return 1;
    }

    if(strcmp(argv[3], "fifo") == 0){
        alg_pagina = FIFO;
    } else{
        alg_pagina = LRU;
    }
    
    FILE *arq = fopen(argv[1], "r");
    if(arq == NULL){
        printf("Erro: Arquivo nao encontrado\n");
        return 1;
    }

    fclose(arq);
    return 0;
}