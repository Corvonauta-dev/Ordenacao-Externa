#ifndef HEAP_MINIMO_H
#define HEAP_MINIMO_H

#include "registro.h"

/**
 * ▪ NoHeap:
 *   • registro (RegistroDisco): nó que armazena o dado
 *   • id_run (size_t): índice da run de origem desse registro
 */
typedef struct {
    RegistroDisco registro; // ➔ dado armazenado no nó
    size_t        id_run;   // ➔ identificador da run de onde veio
} NoHeap;

/**
 * ──────────────────────────────────────────────────────────────────────────────────
 * Funções de heap mínimo para merge k-way usando RegistroDisco.chave como critério
 * ──────────────────────────────────────────────────────────────────────────────────
 */

/**
 * ➔ troca_nos:
 *     Troca dois nós de heap entre si.
 *
 * param a  Ponteiro para o primeiro nó
 * param b  Ponteiro para o segundo nó
 */
void troca_nos(NoHeap *a, NoHeap *b);

/**
 * ➔ descer_heap:
 *     Ajusta (heapify down) a subárvore enraizada em índice i para
 *     manter a propriedade de min-heap (menor chave na raiz).
 *
 * param heap         Array de nós do heap
 * param tamanho      Quantidade atual de nós no heap
 * param i            Índice onde começa a verificação (subárvore)
 */
void descer_heap(NoHeap heap[], size_t tamanho, size_t i);

/**
 * ➔ subir_heap:
 *     Ajusta (heapify up) o nó no índice i até que a propriedade
 *     de min-heap seja restaurada.
 *
 * param heap         Array de nós do heap
 * param i            Índice do nó que deve “subir”
 */
void subir_heap(NoHeap heap[], size_t i);

/**
 * ➔ inicializa_heap:
 *     Inicializa o heap definindo tamanho = 0.
 *
 * param heap         Array de nós (já alocado pelo chamador)
 * param tamanho_ptr  Ponteiro que receberá o tamanho inicial (0)
 */
void inicializa_heap(NoHeap heap[], size_t *tamanho_ptr);

/**
 * ➔ inserir_heap:
 *     Insere um novo nó no min-heap e ajusta posição (“subir”) para
 *     manter a ordem de prioridade.
 *
 * param heap         Array de nós (já alocado)
 * param tamanho_ptr  Ponteiro para a contagem atual de nós (é incrementado)
 * param no           Nó a ser inserido
 */
void inserir_heap(NoHeap heap[], size_t *tamanho_ptr, NoHeap no);

/**
 * ➔ remover_raiz_heap:
 *     Remove e retorna o nó com menor chave (raiz). Ajusta (“descer”)
 *     o heap para manter a propriedade de min-heap.
 *
 * param heap         Array de nós (já alocado)
 * param tamanho_ptr  Ponteiro para a contagem atual de nós (é decrementado)
 * return             Nó removido (menor chave)
 */
NoHeap remover_raiz_heap(NoHeap heap[], size_t *tamanho_ptr);

#endif // HEAP_MINIMO_H
