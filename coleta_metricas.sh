#!/usr/bin/env bash
set -euo pipefail

### CONFIGURAÇÃO ##############################################################
BIN="./bin/ordenacao-externa"
ARQ="$1"        # ex: misturado-1234.vet ou grande.vet
MAXB="$2"       # ex: 10, 50, 50000 etc.

LOG="exec_$(basename "$ARQ")_${MAXB}.log"

# --- 1) Limpa quaisquer runs antigas/temporários restantes ---
rm -f run_*.bin runInter_*.bin 2>/dev/null

### 2) Inicia o programa, redirecionando stdout+stderr para o terminal E para o ficheiro de log
echo "Executando: $BIN $ARQ $MAXB"
echo "----------------------------------------------------------------------"
# A saída do 'time' (que vai para stderr) e a saída do programa (stdout e stderr)
# são todas enviadas para o 'tee', que as mostra no terminal e grava no ficheiro LOG.
# É importante que as métricas (METRICA_...) impressas em stderr pelo programa C
# também sejam capturadas.
{ LC_ALL=C /usr/bin/time --verbose "$BIN" "$ARQ" "$MAXB"; } 2>&1 | tee "$LOG"

# O script espera aqui porque a linha acima é síncrona (não tem '&' no final)
#-------------------------- Fim da execução ----------------------------#
echo "----------------------------------------------------------------------"
echo
echo "=== Programa terminou. Coletando dados do ficheiro: $LOG ==="
echo

### 3) Extrair métricas do log principal ($LOG) ###

# 3.1. Número de runs geradas na Fase 1
RUNS=$(
  grep "Fase 1 concluída:" "$LOG" \
    | head -n1 \
    | sed -E 's/.*: *([0-9]+) .*/\1/' \
    || echo "0" # Valor de fallback se o grep falhar
)

# 3.2. Tempo de cada fase (agora vindo diretamente do programa C)
if grep -q "METRICA_TEMPO_FASE1" "$LOG"; then
  TEMPO1=$(grep "METRICA_TEMPO_FASE1" "$LOG" | cut -d':' -f2 | tr -d ' ')
else
  TEMPO1="N/A"
fi

if grep -q "METRICA_TEMPO_FASE2" "$LOG"; then
  TEMPO2=$(grep "METRICA_TEMPO_FASE2" "$LOG" | cut -d':' -f2 | tr -d ' ')
else
  TEMPO2="N/A"
fi

if grep -q "METRICA_TEMPO_FASE3" "$LOG"; then
  TEMPO3=$(grep "METRICA_TEMPO_FASE3" "$LOG" | cut -d':' -f2 | tr -d ' ')
else
  TEMPO3="N/A"
fi

# 3.3. I/O: “File system inputs” e “File system outputs” vindos do time --verbose
FS_IN=$(grep -i "File system inputs" "$LOG" | awk '{print $4}' || echo "0")
FS_OUT=$(grep -i "File system outputs" "$LOG" | awk '{print $4}' || echo "0")

# 3.4. Pico de descritores abertos (agora vindo diretamente do programa C)
if grep -q "METRICA_MAX_FD" "$LOG"; then
  MAX_OPEN=$(grep "METRICA_MAX_FD" "$LOG" | cut -d':' -f2 | tr -d ' ')
else
  MAX_OPEN="N/A"
fi

### 4) Contar arquivos temporários que restaram no fim
TMP_COUNT=$(find . -maxdepth 1 \( -name "run_*.bin" -o -name "runInter_*.bin" \) -print 2>/dev/null | wc -l)

### 5) Impressão final consolidada
echo "=== Métricas para $(basename "$ARQ") (max_blocos=$MAXB) ==="
echo "Número de runs (Fase 1)         : $RUNS"
echo "Tempo Fase 1 (s)                : $TEMPO1"
echo "Tempo Fase 2 (s)                : $TEMPO2"
echo "Tempo Fase 3 (s)                : $TEMPO3"
echo "I/O Leitura (bytes do 'time')   : $FS_IN"
echo "I/O Gravação (bytes do 'time')  : $FS_OUT"
echo "Máx. descritores abertos (fd)   : $MAX_OPEN"
echo "Arquivos temporários restantes  : $TMP_COUNT"
echo "Log completo salvo em           : $LOG"
