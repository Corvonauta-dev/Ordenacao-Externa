#ifndef LEITOR_RUN_H
#define LEITOR_RUN_H

#include "registro.h"

/**
 * ▪ LeitorRun:
 *   • arquivo (FILE*): ponteiro para o arquivo binário da run
 *   • registro (RegistroDisco): conteúdo do registro lido atualmente
 *   • tem_reg (bool): indica se ainda há registro disponível (true/false)
 */
typedef struct {
    FILE           *arquivo;   // ➔ handle do arquivo de run
    RegistroDisco   registro;  // ➔ registro que está sendo processado
    bool            tem_reg;   // ➔ true se ainda há algo para ler
} LeitorRun;

/**
 * ➔ inicializa_leitor:
 *     Abre o arquivo “run” (run_xxxxx.bin) para leitura e já lê
 *     o primeiro registro (se existir), sinalizando tem_reg = true.
 *
 * param lr        Ponteiro para a struct LeitorRun a ser inicializada
 * param nome_run  Caminho/nome do arquivo de run (ex.: “run_00012.bin”)
 */
void inicializa_leitor(LeitorRun *lr, const char *nome_run);

/**
 * ➔ avancar_leitor:
 *     Avança para o próximo registro dentro da run. Se chegar ao EOF,
 *     fecha o FILE e seta tem_reg = false.
 *
 * param lr  Ponteiro para LeitorRun já inicializado
 */
void avancar_leitor(LeitorRun *lr);

#endif // LEITOR_RUN_H
