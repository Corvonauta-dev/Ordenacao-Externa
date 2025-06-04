#include <stdlib.h>
#include "heap_minimo.h"

/*
 * ➔ troca_nos:
 *     Troca dois nós de heap entre si.
 */
void troca_nos(NoHeap *a, NoHeap *b) {
    NoHeap temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * ➔ descer_heap:
 *     Ajusta a subárvore de índice i para baixo (heapify down),
 *     garantindo que o menor elemento fique na raiz (min-heap).
 *
 * param heap    Array de nós do heap
 * param tamanho Quantidade de nós no heap
 * param i       Índice onde iniciar a “descida”
 */
void descer_heap(NoHeap heap[], size_t tamanho, size_t i) {
    size_t menor = i;
    size_t esq   = 2 * i + 1;
    size_t dir   = 2 * i + 2;

    if (esq < tamanho && heap[esq].registro.chave < heap[menor].registro.chave) {
        menor = esq;
    }
    if (dir < tamanho && heap[dir].registro.chave < heap[menor].registro.chave) {
        menor = dir;
    }
    if (menor != i) {
        troca_nos(&heap[i], &heap[menor]);
        descer_heap(heap, tamanho, menor);
    }
}

/*
 * ➔ subir_heap:
 *     Ajusta o nó no índice i para cima (heapify up),
 *     restaurando a propriedade de min-heap.
 *
 * param heap  Array de nós do heap
 * param i     Índice do nó que deve “subir”
 */
void subir_heap(NoHeap heap[], size_t i) {
    if (i == 0) return;
    size_t pai = (i - 1) / 2;
    if (heap[i].registro.chave < heap[pai].registro.chave) {
        troca_nos(&heap[i], &heap[pai]);
        subir_heap(heap, pai);
    }
}

/*
 * ➔ inicializa_heap:
 *     Define o tamanho inicial do heap como 0.
 *
 * param heap         Array de nós (alocado pelo chamador)
 * param tamanho_ptr  Ponteiro para o tamanho (recebe 0)
 */
void inicializa_heap(NoHeap heap[], size_t *tamanho_ptr) {
    *tamanho_ptr = 0;
}

/*
 * ➔ inserir_heap:
 *     Insere um novo nó no final do array e ajusta para cima.
 *
 * param heap         Array de nós (alocado pelo chamador)
 * param tamanho_ptr  Ponteiro para o tamanho atual (incrementado)
 * param no           Nó a ser inserido
 */
void inserir_heap(NoHeap heap[], size_t *tamanho_ptr, NoHeap no) {
    size_t i = *tamanho_ptr;
    heap[i] = no;
    (*tamanho_ptr)++;
    subir_heap(heap, i);
}

/*
 * ➔ remover_raiz_heap:
 *     Remove e retorna o nó da raiz (menor chave). Move o último
 *     nó para a raiz e faz heapify down para restaurar a ordem.
 *
 * param heap         Array de nós (alocado pelo chamador)
 * param tamanho_ptr  Ponteiro para o tamanho atual (decrementado)
 * return             Nó removido (o menor elemento)
 */
NoHeap remover_raiz_heap(NoHeap heap[], size_t *tamanho_ptr) {
    NoHeap raiz = heap[0];
    heap[0] = heap[*tamanho_ptr - 1];
    (*tamanho_ptr)--;
    if (*tamanho_ptr > 0) {
        descer_heap(heap, *tamanho_ptr, 0);
    }
    return raiz;
}
