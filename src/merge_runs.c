#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "merge_runs.h"
#include "leitor_run.h"
#include "heap_minimo.h"
#include "monitor.h"

/*
 * ➔ comparar_registros:
 *     Função de comparação para qsort. Ordena vetor de RegistroDisco
 *     em memória pelo campo “chave”.
 *
 * param a  Ponteiro genérico para RegistroDisco
 * param b  Ponteiro genérico para RegistroDisco
 * return   • < 0 se a.chave < b.chave
 *           • > 0 se a.chave > b.chave
 *           •   0 se forem iguais
 */
int comparar_registros(const void *a, const void *b) {
    const RegistroDisco *r1 = (const RegistroDisco *) a;
    const RegistroDisco *r2 = (const RegistroDisco *) b;
    if (r1->chave < r2->chave) return -1;
    if (r1->chave > r2->chave) return +1;
    return 0;
}

/*
 * ➔ mesclar_runs_bloco:
 *     Realiza merge k-way de até MAX_RUNS_ABERTAS arquivos de run em disco.
 *
 * Passos:
 * 1) Se n_runs == 0, retorna erro imediatamente.
 * 2) Aloca um array de LeitorRun com n_runs elementos.
 * 3) Para cada run, chama inicializa_leitor() e insere o primeiro registro
 *    disponível no heap (contendo registro + id_run).
 * 4) Abre o arquivo de saída (nome_saida) em “wb”.
 * 5) Enquanto o heap não estiver vazio:
 *       • remover_raiz_heap() → nó com menor chave
 *       • grava registro (menor.registro) no arquivo de saída
 *       • avança o leitor correspondente (id_run) e, se ele ainda
 *         tiver registro, insere novo nó no heap
 * 6) Fecha o arquivo de saída e libera memória.
 *
 * param runs_entrada  Vetor de strings com nomes dos arquivos run_xxxxx.bin
 * param n_runs        Quantidade de runs a mesclar (≤ MAX_RUNS_ABERTAS)
 * param nome_saida    Nome do arquivo de saída (ex.: “runInter_00001.bin”)
 * return              •  0 em sucesso
 *                      • -1 em caso de falha (n_runs == 0, malloc, fopen, fwrite etc.)
 */
int mesclar_runs_bloco(char **runs_entrada, size_t n_runs, const char *nome_saida) {
    // 1) Se não houver runs para mesclar, aborta cedo
    if (n_runs == 0) {
        return -1;
    }

    // 2) Aloca um array de LeitorRun com n_runs elementos
    LeitorRun *leitores = malloc(n_runs * sizeof(LeitorRun));
    if (!leitores) return -1;

    // Inicializa leitores para cada arquivo de run
    for (size_t i = 0; i < n_runs; i++) {
        inicializa_leitor(&leitores[i], runs_entrada[i]);
    }

    // 3) Aloca heap (array de NoHeap) de tamanho n_runs
    NoHeap *heap = malloc(n_runs * sizeof(NoHeap));
    if (!heap) {
        free(leitores);
        return -1;
    }
    size_t tamanho_heap = 0;
    inicializa_heap(heap, &tamanho_heap);

    // Insere o primeiro registro de cada leitor no heap
    for (size_t i = 0; i < n_runs; i++) {
        if (leitores[i].tem_reg) {
            NoHeap no = { .registro = leitores[i].registro, .id_run = i };
            inserir_heap(heap, &tamanho_heap, no);
        }
    }

    // 4) Abre arquivo de saída para gravação binária
    FILE *saida = mon_fopen(nome_saida, "wb");
    if (!saida) {
        free(heap);
        free(leitores);
        return -1;
    }

    // 5) Loop principal: retira o menor elemento e grava no arquivo de saída
    while (tamanho_heap > 0) {
        // Remove raiz (menor chave)
        NoHeap menor = remover_raiz_heap(heap, &tamanho_heap);

        // Grava registro no arquivo de saída
        if (fwrite(&menor.registro, sizeof(RegistroDisco), 1, saida) != 1) {
            mon_fclose(saida);
            free(heap);
            free(leitores);
            return -1;
        }

        // Avança o leitor da run de origem
        size_t id = menor.id_run;
        avancar_leitor(&leitores[id]);

        // Se o leitor ainda tiver registro, insere novo nó no heap
        if (leitores[id].tem_reg) {
            NoHeap novo = { .registro = leitores[id].registro, .id_run = id };
            inserir_heap(heap, &tamanho_heap, novo);
        }
    }

    // 6) Libera recursos
    mon_fclose(saida);
    free(heap);
    free(leitores);
    return 0;
}
