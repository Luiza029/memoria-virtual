# Gerenciador de Memória Virtual (vm)
## Arquivos

- `main.c`: valida os argumentos e chama a tradução dos endereços.
- `vm.c`: valida o arquivo de endereços e traduz cada endereço (TLB com threads, tabela de páginas, page fault, substituição FIFO/LRU e gravação do `correct.txt`).
- `vm.h`: constantes do projeto, a `struct` usada pelas threads da TLB e os protótipos das funções.
- `Makefile`: compila o projeto e gera o executável `vm`.

## Como compilar

```bash
make
```

Para apagar os arquivos gerados (`.o`, `vm` e `correct.txt`):

```bash
make clean
```

## Como executar

```bash
./vm <arquivo_de_enderecos> <fifo|lru> <fifo|lru>
```

- 1º argumento: arquivo com os endereços lógicos (um número por linha, de 0 a 65535);
- 2º argumento: algoritmo de substituição de **página** (`fifo` ou `lru`);
- 3º argumento: algoritmo de substituição da **TLB** (`fifo` ou `lru`).


## Sistema operacional
Desenvolvido e testado em **Ubuntu 26.04 LTS** rodando no **WSL2** (Windows 11), com **gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0** e GNU Make.

*Os arquivos `addresses.txt` e `BACKING_STORE.bin`, fornecidos pelo professor, precisam estar na mesma pasta do executável.*