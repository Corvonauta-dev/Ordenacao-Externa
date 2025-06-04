#ifndef MERGE_RUNS_H
#define MERGE_RUNS_H

#include "registro.h"

/**
 * ➔ comparar_registros:
 *     Função de comparação usada pelo qsort para ordenar registros
 *     em memória pelo campo “chave”.
 *
 * param a  Ponteiro para o primeiro RegistroDisco
 * param b  Ponteiro para o segundo RegistroDisco
 * return   • < 0 se a.chave < b.chave
 *           • > 0 se a.chave > b.chave
 *           •   0 se forem iguais
 */
int comparar_registros(const void *a, const void *b);

/**
 * ➔ mesclar_runs_bloco:
 *     Faz o “k-way merge” de até MAX_RUNS_ABERTAS arquivos de run
 *     em um único arquivo de saída ordenado.  
 *
 *   Passos principais:
 *   1) Aloca n_runs leitores (LeitorRun) e abre cada run para leitura.  
 *   2) Cria um min-heap (array de NoHeap) e insere o primeiro registro
 *      de cada run, junto do índice da run.  
 *   3) Enquanto o heap não estiver vazio:
 *        a) remove_raiz_heap → nó com menor chave  
 *        b) grava registro no arquivo de saída  
 *        c) avança o leitor correspondente (id_run) e, se ele ainda
 *           tiver registro, insere novo nó no heap  
 *   4) Fecha o arquivo de saída e libera memória.  
 *
 * param runs_entrada  Vetor de strings (nomes dos arquivos run_xxxxx.bin)
 * param n_runs        Quantidade de runs a mesclar (≤ MAX_RUNS_ABERTAS)
 * param nome_saida    Nome do arquivo de saída (ex.: “runInter_00001.bin”)
 * return              •  0 em sucesso  
 *                      • -1 em caso de falha (malloc, fopen, fwrite etc.)
 */
int mesclar_runs_bloco(char **runs_entrada, size_t n_runs, const char *nome_saida);

#endif // MERGE_RUNS_H
