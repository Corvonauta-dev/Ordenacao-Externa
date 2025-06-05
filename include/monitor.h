#ifndef MONITOR_H
#define MONITOR_H

#include <stdio.h> // Necessário para a declaração de FILE

/*
 * Interface da biblioteca de monitoramento para o projeto de ordenação externa.
 * Mede o tempo de execução de fases e o pico de uso de descritores de arquivo.
 */

// --- Funções de Monitoramento de Arquivos ---

/**
 * brief Um invólucro para fopen() que monitora o número de arquivos abertos.
 * Use esta função em vez de fopen() para contar os descritores.
 */
FILE *mon_fopen(const char *pathname, const char *mode);

/**
 * brief Um invólucro para fclose() que monitora o número de arquivos abertos.
 * Use esta função em vez de fclose().
 */
int mon_fclose(FILE *stream);

/**
 * brief Imprime a métrica do pico de descritores de arquivo abertos.
 * Chame no final da função main().
 */
void mon_log_max_fd(void);


// --- Funções de Monitoramento de Tempo ---

/**
 * brief Inicia um cronômetro para medir a duração de uma fase.
 */
void mon_timer_start(void);

/**
 * brief Para o cronômetro e imprime o tempo decorrido para a fase especificada.
 * param phase_num O número da fase (1, 2, 3, etc.) a ser impresso no log.
 */
void mon_timer_stop_and_log(int phase_num);


size_t mon_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t mon_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
void mon_log_io_stats(void);

#endif // MONITOR_H