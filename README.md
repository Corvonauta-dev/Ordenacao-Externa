# Ordenação Externa e Reconstrução de Arquivos TAR com K-Way Merge

Este repositório contém a implementação em C de um algoritmo de ordenação externa projetado para processar grandes volumes de dados, especificamente focando na ordenação de registos e na reconstrução de Arquivos no formato TAR. O projeto demonstra uma abordagem clássica utilizando a criação de *runs* ordenadas e a sua subsequente intercalação (*k-way merge*) otimizada por um *heap* de mínimo.


## Visão Geral do Projeto

A ordenação externa é um problema fundamental na ciência da computação que surge quando o volume de dados a ser ordenado excede a capacidade da memória principal (RAM). Este projeto implementa uma solução para este problema, focando na eficiência de I/O e na gestão de memória. A aplicação específica é a ordenação de registos extraídos de um Arquivo de dados (`.vet`) e a subsequente reconstrução de um Arquivo TAR original, garantindo a sua integridade.

## Objetivo

O principal objetivo deste trabalho é implementar e avaliar experimentalmente um algoritmo de ordenação externa, demonstrando o impacto de diferentes tamanhos de buffer de memória (`max_blocos`) no desempenho global. O estudo também visa:

* Validar a correção funcional do algoritmo através da reconstrução de Arquivos TAR.
* Medir com precisão o volume de I/O lógico e o uso de recursos do sistema.
* Analisar o comportamento do algoritmo em diferentes fases e identificar gargalos.

## Metodologia

O algoritmo é implementado em C e dividido em três fases principais:

### Fase 1: Criação de Runs Ordenadas

O Arquivo de entrada (`.vet`) é lido em blocos, cujo tamanho é determinado pelo parâmetro `max_blocos` (número máximo de registos que o buffer em RAM pode conter). Cada bloco é ordenado internamente na memória usando o algoritmo Quicksort. As sequências ordenadas resultantes, chamadas *runs*, são gravadas em Arquivos temporários no disco.

### Fase 2: Intercalação K-Way Merge

As *runs* geradas na Fase 1 são intercaladas para produzir um único Arquivo totalmente ordenado. Este processo utiliza a técnica de *k-way merge* (intercalação de k vias), onde `k` é o número máximo de Arquivos de *run* que podem ser abertos simultaneamente (definido pela constante `MAX_RUNS_ABERTAS`, tipicamente 500). Um *heap* de mínimo é empregado para selecionar eficientemente o próximo registo a ser escrito na *run* de saída. Se o número de *runs* iniciais exceder `k`, a intercalação é realizada em múltiplas passagens, reduzindo o número de Arquivos a cada passagem até que reste apenas um.

### Fase 3: Reconstrução do Arquivo TAR

A partir do Arquivo final ordenado (contendo todos os registos na ordem correta), o Arquivo TAR original é reconstruído. Isto envolve concatenar os dados úteis de cada registo e garantir o correto alinhamento e *padding* conforme as especificações do formato TAR, incluindo os blocos finais de zeros.

### Monitorização e Métricas

Uma biblioteca de monitorização interna (`monitor.c` e `monitor.h`) foi desenvolvida para coletar métricas precisas, incluindo:

* Tempo de execução para cada fase.
* Volume total de I/O lógico (bytes lidos e escritos pelas funções `fread`/`fwrite`).
* Pico de descritores de Arquivo (FD) abertos simultaneamente.

## Desafio de Alinhamento TAR

Um desafio significativo encontrado foi garantir o correto alinhamento dos blocos de dados para a reconstrução válida de Arquivos TAR. Registos processados com um tamanho que não resultava em múltiplos de 512 bytes ao final da reconstrução impediam o reconhecimento correto do Arquivo. Este problema foi solucionado ajustando a estrutura dos registos com a adição de *padding*, garantindo o alinhamento adequado.

## Estrutura do Repositório


## Estrutura do Repositório


```bash
.
├── Makefile                # Script para compilação do projeto
├── coleta_metricas.sh      # Script para executar testes e coletar métricas
├── bin/                    # Diretório para o executável compilado
│   └── ordenacao-externa
├── include/                # Diretório para Arquivos de cabeçalho (.h)
│   ├── heap_minimo.h
│   ├── leitor_run.h
│   ├── merge_runs.h
│   ├── monitor.h
│   ├── quicksort.h
│   └── registro.h
├── obj/                    # Diretório para Arquivos objeto (.o) (criado pelo Makefile)
├── src/                    # Diretório para Arquivos fonte (.c)
│   ├── heap_minimo.c
│   ├── leitor_run.c
│   ├── main.c
│   ├── merge_runs.c
│   ├── monitor.c
│   └── quicksort.c
├──  misturado-1234.vet
├──  grande.vet                  
└── README.md               # Este Arquivo
```

## Como Compilar

O projeto utiliza um `Makefile` para facilitar a compilação. Para compilar, navegue até o diretório raiz do projeto no seu terminal e execute:

```bash
make
```

Isto irá compilar todos os Arquivos fonte e criar o executável `bin/ordenacao-externa`.
Para limpar os Arquivos compilados (objetos e executável), use:

```bash
make clean
```

## Como Executar e Coletar Métricas

### Usando o Script `coleta_metricas.sh`

A forma recomendada de executar os testes e coletar métricas detalhadas é através do script `coleta_metricas.sh`.

```bash
chmod +x coleta_metricas.sh
./coleta_metricas.sh <Arquivo_de_entrada.vet> <max_blocos>
```

**Exemplo:**

```bash
./coleta_metricas.sh dados/grande.vet 50000
```

O script executará o programa de ordenação, exibirá o progresso no terminal e salvará um log detalhado (incluindo a saída do `/usr/bin/time --verbose` e as métricas da biblioteca `monitor.c`) num Arquivo chamado `exec_<nome_Arquivo>_<max_blocos>.log`. No final, um resumo das métricas será impresso no terminal.

### Executando o Programa Diretamente

Você também pode executar o programa compilado diretamente:

```bash
./bin/ordenacao-externa <Arquivo_de_entrada.vet> <max_blocos>
```

**Exemplo:**

```bash
./bin/ordenacao-externa dados/grande.vet 50000
```

Neste caso, as mensagens de progresso e as métricas da biblioteca `monitor.c` (impressas em `stderr`) aparecerão no terminal, mas a saída do `/usr/bin/time` não será coletada automaticamente.

## Resultados Experimentais (Resumo)

Os experimentos, conduzidos com um Arquivo de teste de 6.4 GB (`grande.vet`), demonstraram que:

* O **tamanho do buffer de memória (`max_blocos`)** é um fator crucial para o desempenho.
* Aumentar `max_blocos` de 50 para 500.000 reduziu o tempo total de execução de mais de 4 minutos para aproximadamente 1 minuto e 6 segundos.
* A principal otimização ocorre na Fase 2 (intercalação), devido à redução do número de *runs* iniciais e, consequentemente, do número de passagens de intercalação.
  * Com `max_blocos=50`, foram necessárias 3 passagens.
  * Com `max_blocos=5000`, 2 passagens.
  * Com `max_blocos=50000` ou mais, apenas 1 passagem.
* O **I/O Lógico** total processado diminuiu de ~61 GB para ~36.5 GB ao otimizar o `max_blocos`.
* O pico de **Descritores de Arquivo** utilizados alinhou-se com a teoria, variando de 504 (para `max_blocos` pequenos) a 53 (para `max_blocos` grandes).
* Foi identificado um ponto de **retornos decrescentes**, onde aumentar `max_blocos` para além do necessário para uma única passagem de intercalação traz ganhos marginais.
* A **correção funcional** foi validada em todos os testes através da comparação de hash SHA-256.

Para uma análise detalhada, consulte o artigo completo [link para o seu artigo, se aplicável].

## Figuras Ilustrativas

O artigo associado a este projeto inclui figuras que ilustram:

1. O impacto do `max_blocos` no tempo total de execução.
2. O volume de I/O Lógico versus `max_blocos`.

## Autor

**Luiz** Guilherme Monteiro Padilha

* Estudante de Mestrado, Programa de Pós-Graduação em Ciência da Computação (PPGCC), Universidade Tecnológica Federal do Paraná - UTFPR.
* E-mail: `luipad@alunos.utfpr.edu.br`

## Referências Principais

* Ziviani, N. (2010). *Projeto de Algoritmos: com implementações em Pascal e C*. 3ª ed. Elsevier.
* Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction* to *Algorithms*. 3rd ed. MIT Press.


