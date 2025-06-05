// Para expor clock_gettime e CLOCK_MONOTONIC (funcionalidades POSIX)
#define _POSIX_C_SOURCE 199309L
// Poderia também usar 200809L para uma versão mais recente do POSIX

#include "monitor.h"
#include <stdio.h>
#include <time.h> // Este deve vir DEPOIS da definição de _POSIX_C_SOURCE

/*
 * Implementação da biblioteca de monitoramento.
 * As variáveis de contagem são estáticas para serem privadas a este arquivo.
 */

// --- Estado Interno do Monitor de Arquivos ---
static int g_fd_count = 0;
static int g_max_fd = 0;

// --- Estado Interno do Monitor de Tempo ---
static struct timespec g_timer_start_ts;


// --- Implementação das Funções ---

FILE *mon_fopen(const char *pathname, const char *mode) {
    FILE *f = fopen(pathname, mode);
    if (f != NULL) {
        g_fd_count++;
        if (g_fd_count > g_max_fd) {
            g_max_fd = g_fd_count;
        }
    }
    return f;
}

int mon_fclose(FILE *stream) {
    // A contagem só deve ser decrementada se fclose for bem-sucedido (retorna 0)
    int ret = fclose(stream);
    if (ret == 0) {
        g_fd_count--;
    }
    return ret;
}

void mon_log_max_fd(void) {
    // Adiciona os 3 descritores padrão (stdin, stdout, stderr) que estão sempre abertos.
    fprintf(stderr, "METRICA_MAX_FD: %d\n", g_max_fd + 3);
}

void mon_timer_start(void) {
    clock_gettime(CLOCK_MONOTONIC, &g_timer_start_ts);
}

void mon_timer_stop_and_log(int phase_num) {
    struct timespec timer_end_ts;
    clock_gettime(CLOCK_MONOTONIC, &timer_end_ts);

    double time_taken = (double)(timer_end_ts.tv_sec - g_timer_start_ts.tv_sec) +
                        (double)(timer_end_ts.tv_nsec - g_timer_start_ts.tv_nsec) / 1e9;

    fprintf(stderr, "METRICA_TEMPO_FASE%d: %.4f\n", phase_num, time_taken);
}
