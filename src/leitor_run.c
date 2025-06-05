#include <stdlib.h>
#include <stdio.h>
#include "leitor_run.h"
#include "monitor.h"

/*
 * ➔ inicializa_leitor:
 *     Abre o arquivo binário “run” e já lê o primeiro registro
 *     para preencher lr->registro. Se não conseguir abrir ou
 *     não ler nenhum registro, define tem_reg = false.
 *
 * param lr        Ponteiro para struct LeitorRun a ser inicializada
 * param nome_run  Nome do arquivo de run (ex.: “run_00005.bin”)
 */
void inicializa_leitor(LeitorRun *lr, const char *nome_run) {
    lr->arquivo = mon_fopen(nome_run, "rb");
    if (!lr->arquivo) {
        lr->tem_reg = false;  // ❌ não abriu
        return;
    }
    size_t lidos = mon_fread(&lr->registro, sizeof(RegistroDisco), 1, lr->arquivo);
    if (lidos == 1) {
        lr->tem_reg = true;   // ✔ primeiro registro lido com sucesso
    } else {
        lr->tem_reg = false;  // ❌ sem registros
        mon_fclose(lr->arquivo);
        lr->arquivo = NULL;
    }
}

/*
 * ➔ avancar_leitor:
 *     Avança para o próximo registro na run. Se EOF,
 *     fecha o arquivo e marca tem_reg = false.
 *
 * param lr  Ponteiro para LeitorRun já inicializado
 */
void avancar_leitor(LeitorRun *lr) {
    if (!lr->arquivo) return;  // já estava fechado
    size_t lidos = mon_fread(&lr->registro, sizeof(RegistroDisco), 1, lr->arquivo);
    if (lidos == 1) {
        lr->tem_reg = true;   // ✔ registro válido disponível
    } else {
        lr->tem_reg = false;  // ❌ fim do arquivo
        mon_fclose(lr->arquivo);
        lr->arquivo = NULL;
    }
}
