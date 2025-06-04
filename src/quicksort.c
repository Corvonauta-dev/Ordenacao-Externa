#include <stddef.h>   // para size_t
#include "quicksort.h"

/*
 * ➔ compara_chave:
 *     Função auxiliar que compara duas chaves de RegistroDisco
 *     (mesma lógica do comparar_registros, mas direto sobre uint64_t).
 *
 * param a  valor de chave do primeiro registro
 * param b  valor de chave do segundo registro
 * return   • -1 se a < b
 *           • +1 se a > b
 *           •  0 se a == b
 */
static int compara_chave(uint64_t a, uint64_t b) {
    if (a < b) return -1;
    if (a > b) return +1;
    return 0;
}

/*
 * ➔ troca_dois:
 *     Troca dois elementos no array de RegistroDisco.
 *
 * param v   Ponteiro para a base do array
 * param i   Índice do primeiro elemento
 * param j   Índice do segundo elemento
 */
static void troca_dois(RegistroDisco *v, size_t i, size_t j) {
    RegistroDisco tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
}

/*
 * ➔ particao:
 *     Faz a partição de Lomuto no trecho v[esq..dir], escolhendo pivô
 *     como v[dir]. Após a partição:
 *       • todos os elementos ≤ pivô ficam em v[esq..p]
 *       • pivô é colocado em v[p]
 *       • todos os elementos > pivô ficam em v[p+1..dir]
 *
 * param v    Ponteiro para o array de RegistroDisco
 * param esq  Índice de início da partição
 * param dir  Índice de fim da partição (inclusive) → pivô em v[dir]
 * return     Posição final do pivô (índice p)
 */
static size_t particao(RegistroDisco *v, size_t esq, size_t dir) {
    // Escolhe pivô como o último elemento do trecho
    uint64_t pivô = v[dir].chave;
    // i será o índice final de elementos menores ou iguais ao pivô
    size_t i = esq;

    // Percorre de esq até dir-1
    for (size_t j = esq; j < dir; j++) {
        if (compara_chave(v[j].chave, pivô) <= 0) {
            // v[j].chave <= pivô → troca v[i] com v[j]
            troca_dois(v, i, j);
            i++;
        }
    }
    // Finalmente, coloca o pivô na posição i (troca v[i] e v[dir])
    troca_dois(v, i, dir);
    return i;
}

/*
 * ➔ quicksort_rec:
 *     Função recursiva que aplica QuickSort no array v[esq..dir].
 *
 * param v    Ponteiro para array de RegistroDisco
 * param esq  Índice de início
 * param dir  Índice de fim
 */
static void quicksort_rec(RegistroDisco *v, size_t esq, size_t dir) {
    if (esq < dir) {
        // Particiona e obtém índice do pivô
        size_t p = particao(v, esq, dir);
        // Ordena recursivamente as duas metades
        if (p > 0) {
            // evita underflow quando p == 0
            quicksort_rec(v, esq, p - 1);
        }
        quicksort_rec(v, p + 1, dir);
    }
}

/*
 * ➔ quicksort_registros:
 *     Função pública: chama a recursiva se o trecho for não vazio.
 */
void quicksort_registros(RegistroDisco *vetor, size_t esq, size_t dir) {
    if (vetor == NULL) return;
    if (esq < dir) {
        quicksort_rec(vetor, esq, dir);
    }
}
