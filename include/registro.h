#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ────────────────────────────────────────────────────────────────────────────
// ► Definição do tamanho máximo de runs abertas simultaneamente
#define MAX_RUNS_ABERTAS 500

#pragma pack(push, 1)
/**
 * ▪ RegistroDisco:
 *   ┌─────────────────────────────────────────────────────────────────────────┐
 *   │ • chave (uint64_t):    8 bytes → chave de ordenação                     │
 *   │ • tamanho (uint32_t):  4 bytes → número de bytes válidos em 'pacote'    │
 *   │ • pacote[250]:         250 bytes → dados úteis                          │
 *   │ • preenchimento[2]:    2 bytes → completar 264 bytes totais             │
 *   └─────────────────────────────────────────────────────────────────────────┘
 *   Total: 264 bytes (cada registro)
 */
typedef struct {
    uint64_t       chave;             // ➔ chave de ordenação
    uint32_t       tamanho;           // ➔ bytes válidos em 'pacote'
    unsigned char  pacote[250];       // ➔ dados úteis
    unsigned char  preenchimento[2];  // ➔ completar 264 bytes
} RegistroDisco;
#pragma pack(pop)

// sizeof(RegistroDisco) == 264

#endif // REGISTRO_H
