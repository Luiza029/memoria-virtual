#ifndef VM_H
#define VM_H
#define FIFO 0
#define LRU 1
#define MAX 65535
#define TAM_TLB       16
#define NUM_PAGINAS  256
#define TAM_PAGINA   256
#define NUM_FRAMES   128

typedef struct TLB{
    int pagina;
    int linha;
    int achou;
    int *tlbPaginas;
} TLB;


int validar_enderecos(FILE *arq, int alg_pagina, int alg_tlb);
void *ajudante(void *arg);
#endif