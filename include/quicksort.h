#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "registro.h"

/**
 * ➔ quicksort_registros:
 *     Ordena em memória o array de RegistroDisco de índice [esq…dir]
 *     usando algoritmo de QuickSort baseado em partição de Lomuto.
 *
 * param vetor  Ponteiro para o primeiro elemento do array de RegistroDisco
 * param esq    Índice inicial da região a ordenar (geralmente 0)
 * param dir    Índice final da região a ordenar (geralmente lidos−1)
 */
void quicksort_registros(RegistroDisco *vetor, size_t esq, size_t dir);

#endif // QUICKSORT_H
