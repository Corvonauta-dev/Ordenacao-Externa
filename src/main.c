#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include "registro.h"
#include "merge_runs.h"
#include "quicksort.h"
#include "monitor.h"

/*
 * ────────────────────────────────────────────────────────────────────────────
 * ▪ Função principal (main):
 *   Orquestra as três fases da ordenação externa:
 *
 *   Fase 1: Geração de runs simples (blocos de até max_blocos registros) ➔
 *           lê do arquivo de entrada “misturado-grande.vet” em fatias,
 *           ordena cada fatia em memória e grava run_xxxxx.bin no disco.
 *
 *   Fase 2: Mesclagem em múltiplas passagens (multi-pass merge) ➔
 *           enquanto houver mais de MAX_RUNS_ABERTAS runs no disco:
 *             • agrupa até MAX_RUNS_ABERTAS runs de cada vez
 *             • mescla (k-way merge) em runInter_xxxxx.bin
 *             • remove runs antigas após mesclar
 *           • ao final, mescla o que restar em “grande_sorted.bin”
 *
 *   Fase 3: Reconstrução do arquivo .tar ➔
 *           • lê registros de “grande_sorted.bin”
 *           • grava apenas os bytes válidos (registro.pacote[0..tamanho-1])
 *             em “reconstruido.tar”
 *           • preenche zeros para fechar o último bloco de 512 bytes
 *           • escreve dois blocos de 512 bytes de zeros (fim-de-tar)
 *           • remove todos os arquivos temporários de run_*.bin, runInter_*.bin
 *
 *   Após isso, exibe mensagem de sucesso e finaliza.
 * ────────────────────────────────────────────────────────────────────────────
 */

int main(int argc, char *argv[]) {
    // Checa parâmetros de linha de comando
    if (argc < 3) {
        fprintf(stderr,
                "Uso: %s <arquivo_entrada> <max_blocos_em_memoria>\n"
                "  <arquivo_entrada>       : ex.: misturado-grande.vet\n"
                "  <max_blocos_em_memoria> : quantos registros (264 bytes cada)\n"
                "                            cabem em RAM simultaneamente.\n",
                argv[0]
        );
        return EXIT_FAILURE;
    }

    const char *nome_entrada = argv[1];
    errno = 0;
    long tmp = strtol(argv[2], NULL, 10);
    if (errno != 0 || tmp <= 0) {
        fprintf(stderr, "Erro: <max_blocos_em_memoria> deve ser inteiro positivo.\n");
        return EXIT_FAILURE;
    }
    size_t max_blocos = (size_t) tmp;

    mon_timer_start();

    // ──────────── FASE 1: Criação de runs simples ────────────
    FILE *arquivo_entrada = mon_fopen(nome_entrada, "rb");
    if (!arquivo_entrada) {
        perror("❌ Erro ao abrir arquivo de entrada");
        return EXIT_FAILURE;
    }

    // Aloca buffer para até max_blocos registros em memória
    RegistroDisco *buffer = malloc(max_blocos * sizeof(RegistroDisco));
    if (!buffer) {
        perror("❌ Falha no malloc do buffer");
        mon_fclose(arquivo_entrada);
        return EXIT_FAILURE;
    }

    size_t contagem_runs = 0;
    while (1) {
        // Lê no máximo max_blocos registros de uma vez
        size_t lidos = mon_fread(buffer, sizeof(RegistroDisco), max_blocos, arquivo_entrada);
        if (lidos == 0) break;  // fim de arquivo

        // Ordena em memória pelo campo “chave”
        //qsort(buffer, lidos, sizeof(RegistroDisco), comparar_registros);
        quicksort_registros(buffer, 0, (lidos > 0 ? lidos - 1 : 0));

        // Grava run temporária no disco: run_00000.bin, run_00001.bin, etc.
        char nome_run[64];
        snprintf(nome_run, sizeof(nome_run), "run_%05zu.bin", contagem_runs);
        FILE *saida_run = mon_fopen(nome_run, "wb");
        if (!saida_run) {
            perror("❌ Erro ao criar run temporário");
            free(buffer);
            mon_fclose(arquivo_entrada);
            return EXIT_FAILURE;
        }
        if (mon_fwrite(buffer, sizeof(RegistroDisco), lidos, saida_run) != lidos) {
            perror("❌ Erro ao escrever run temporário");
            free(buffer);
            mon_fclose(arquivo_entrada);
            mon_fclose(saida_run);
            return EXIT_FAILURE;
        }
        mon_fclose(saida_run);
        contagem_runs++;
    }

    mon_fclose(arquivo_entrada);
    free(buffer);

    mon_timer_stop_and_log(1);

    if (contagem_runs == 0) {
        fprintf(stderr, "❌ Nenhum registro encontrado em '%s'.\n", nome_entrada);
        return EXIT_FAILURE;
    }
    printf("✔ Fase 1 concluída: %zu runs simples geradas.\n", contagem_runs);

    mon_timer_start();

    // ──────────── FASE 2: Mesclagem multi-pass ────────────
    // Prepara vetor com nomes de runs atuais (“run_00000.bin”, …)
    char **runs_atuais = malloc(contagem_runs * sizeof(char *));
    if (!runs_atuais) {
        fprintf(stderr, "❌ Falha no malloc de runs_atuais\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < contagem_runs; i++) {
        runs_atuais[i] = malloc(32);
        snprintf(runs_atuais[i], 32, "run_%05zu.bin", i);
    }
    size_t qtd_runs_atuais = contagem_runs;
    size_t passada = 0;

    // Enquanto houver mais runs do que MAX_RUNS_ABERTAS, faz uma passagem de mesclagem
    while (qtd_runs_atuais > MAX_RUNS_ABERTAS) {
        // Calcula quantos blocos (chunks) de até MAX_RUNS_ABERTAS serão mesclados
        size_t n_blocos = (qtd_runs_atuais + MAX_RUNS_ABERTAS - 1) / MAX_RUNS_ABERTAS;
        char **prox_runs = malloc(n_blocos * sizeof(char *));
        if (!prox_runs) {
            fprintf(stderr, "❌ Falha no malloc de prox_runs\n");
            return EXIT_FAILURE;
        }

        for (size_t bloco = 0; bloco < n_blocos; bloco++) {
            // Determina início/fim do bloco de runs a mesclar nesta iteração
            size_t inicio = bloco * MAX_RUNS_ABERTAS;
            size_t fim    = inicio + MAX_RUNS_ABERTAS;
            if (fim > qtd_runs_atuais) fim = qtd_runs_atuais;
            size_t qtd = fim - inicio;

            // Nomeia run intermediária: runInter_XXXXX.bin (passada * 1000 + bloco)
            prox_runs[bloco] = malloc(32);
            snprintf(prox_runs[bloco], 32,
                     "runInter_%05zu.bin", passada * 1000 + bloco);

            // Faz merge das runs_atuais[inicio..fim-1] em prox_runs[bloco]
            if (mesclar_runs_bloco(&runs_atuais[inicio], qtd, prox_runs[bloco]) < 0) {
                fprintf(stderr, "❌ Erro em mesclar_runs_bloco (passada %zu, bloco %zu)\n", passada, bloco);
                return EXIT_FAILURE;
            }

            // Apaga os arquivos de run antigos já mesclados
            for (size_t k = inicio; k < fim; k++) {
                remove(runs_atuais[k]);
                free(runs_atuais[k]);
            }
        }

        // Prepara o vetor runs_atuais para a próxima passada
        free(runs_atuais);
        runs_atuais = prox_runs;
        qtd_runs_atuais = n_blocos;
        passada++;
        printf("✔ Passada %zu concluída: agora %zu runs intermediárias.\n", passada, qtd_runs_atuais);
    }

    // Último merge (restantes ≤ MAX_RUNS_ABERTAS) → “grande_sorted.bin”
    const char *nome_ordenado = "grande_sorted.bin";
    if (mesclar_runs_bloco(runs_atuais, qtd_runs_atuais, nome_ordenado) < 0) {
        fprintf(stderr, "❌ Erro no merge final de %zu runs.\n", qtd_runs_atuais);
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < qtd_runs_atuais; i++) {
        remove(runs_atuais[i]);
        free(runs_atuais[i]);
    }
    free(runs_atuais);

    mon_timer_stop_and_log(2);

    printf("✔ Fase 2 concluída: arquivo “%s” gerado.\n", nome_ordenado);

    mon_timer_start();

    // ──────────── FASE 3: Reconstrução em “reconstruido.tar” ────────────
    FILE *arquivo_ordenado = mon_fopen(nome_ordenado, "rb");
    if (!arquivo_ordenado) {
        perror("❌ Erro ao abrir “grande_sorted.bin” para reconstrução");
        return EXIT_FAILURE;
    }
    const char *nome_recon = "reconstruido.tar";
    FILE *saida_recon = mon_fopen(nome_recon, "wb");
    if (!saida_recon) {
        perror("❌ Erro ao criar “reconstruido.tar”");
        mon_fclose(arquivo_ordenado);
        return EXIT_FAILURE;
    }

    // Lê registro a registro de “grande_sorted.bin”
    RegistroDisco reg_temp;
    size_t total_bytes = 0;
    while (mon_fread(&reg_temp, sizeof(RegistroDisco), 1, arquivo_ordenado) == 1) {
        if (reg_temp.tamanho > 250) {
            reg_temp.tamanho = 250;  // segurança extra
        }
        // Grava apenas pacote[0..tamanho-1] no .tar reconstruído
        if (mon_fwrite(reg_temp.pacote, 1, reg_temp.tamanho, saida_recon) != reg_temp.tamanho) {
            perror("❌ Erro ao escrever em “reconstruido.tar”");
            mon_fclose(arquivo_ordenado);
            mon_fclose(saida_recon);
            return EXIT_FAILURE;
        }
        total_bytes += reg_temp.tamanho;
    }

    // 1) Padding para fechar o último bloco de 512 bytes
    size_t resto = total_bytes % 512;
    size_t pad = (resto == 0) ? 0 : (512 - resto);
    if (pad > 0) {
        unsigned char *zeros = calloc(pad, 1);
        if (!zeros) {
            perror("❌ Falha no calloc do padding");
            mon_fclose(arquivo_ordenado);
            mon_fclose(saida_recon);
            return EXIT_FAILURE;
        }
        mon_fwrite(zeros, 1, pad, saida_recon);
        free(zeros);
    }

    // 2) Dois blocos de 512 bytes de zeros (fim-de-tar)
    unsigned char bloco_zero[512] = { 0 };
    mon_fwrite(bloco_zero, 1, 512, saida_recon);
    mon_fwrite(bloco_zero, 1, 512, saida_recon);

    mon_fclose(arquivo_ordenado);
    mon_fclose(saida_recon);

    mon_timer_stop_and_log(3);

    printf("✔ Fase 3 concluída: “%s” gerado (conteúdo = %zu bytes + padding = %zu bytes).\n",
           nome_recon, total_bytes, pad + 1024);
    printf("✔ Ordenação Externa finalizada com sucesso!\n");

    // ──────────── LIMPEZA FINAL: remove arquivos temporários ────────────
    DIR *dir = opendir(".");
    if (dir) {
        struct dirent *entrada;
        while ((entrada = readdir(dir)) != NULL) {
            // Apaga tudo que começa com “run_” ou “runInter_”
            if (strncmp(entrada->d_name, "run_", 4) == 0 ||
                strncmp(entrada->d_name, "runInter_", 9) == 0) {
                if (remove(entrada->d_name) != 0) {
                    fprintf(stderr,
                            "⚠️ Aviso: falha ao apagar temporário “%s”\n",
                            entrada->d_name);
                }
            }
        }
        closedir(dir);
    }

    mon_log_max_fd();
    mon_log_io_stats();

    return EXIT_SUCCESS;
}