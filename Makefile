# Nome do compilador
CC        := gcc
# Flags de compilação:
# -std=c11: Usa o padrão C11
# -O2: Nível de otimização 2
# -Wall -Wextra -pedantic: Ativa muitos avisos úteis
# -Iinclude: Diz ao compilador para procurar por ficheiros .h na pasta 'include'
CFLAGS    := -std=c11 -O2 -Wall -Wextra -pedantic -Iinclude
# Flags de ligação:
# -lrt: Liga com a biblioteca de tempo real (para clock_gettime)
LDFLAGS   := -lrt

# Diretórios
SRCDIR    := src
OBJDIR    := obj
BINDIR    := bin

# Arquivos fonte
# Todos os ficheiros .c, incluindo main.c, estão na pasta SRCDIR (src/)
SOURCE_FILES := $(wildcard $(SRCDIR)/*.c)

# Arquivos objeto (.o)
# Converte src/ficheiro.c -> obj/ficheiro.o para todos os ficheiros fonte
OBJECT_FILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCE_FILES))

# Nome do executável final
TARGET  := $(BINDIR)/ordenacao-externa

# Regra padrão: construir o executável
all: $(TARGET)

# Regra para ligar todos os ficheiros objeto no executável final
# Pré-requisitos: todos os $(OBJECT_FILES) e o diretório $(BINDIR)
# O '$^' representa todos os pré-requisitos ($(OBJECT_FILES))
# O '$@' representa o alvo (o nome do executável)
$(TARGET): $(OBJECT_FILES) | $(BINDIR)
	@echo "Ligando o executável..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Executável '$(TARGET)' criado com sucesso."

# Regra genérica para compilar qualquer ficheiro .c da pasta SRCDIR (src/)
# num ficheiro .o correspondente na pasta OBJDIR (obj/)
# Ex: compila src/main.c para obj/main.o, src/monitor.c para obj/monitor.o, etc.
# Pré-requisitos: o ficheiro .c correspondente em src/ e o diretório $(OBJDIR)
# O '$<' representa o primeiro pré-requisito (o ficheiro .c)
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "Compilando $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para criar os diretórios OBJDIR e BINDIR se eles não existirem
# Estas são "order-only prerequisites" (indicadas pelo '|'),
# o que significa que as pastas são verificadas/criadas antes das regras
# que dependem delas, mas não causam recompilação se os seus timestamps mudarem.
$(OBJDIR):
	@echo "Criando diretório $(OBJDIR)..."
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@echo "Criando diretório $(BINDIR)..."
	@mkdir -p $(BINDIR)

# Regra para limpar os artefactos de compilação
clean:
	rm -f $(OBJECT_FILES) $(TARGET)
	@echo "Ficheiros objeto e executável removidos."
	@echo "Os diretórios '$(OBJDIR)' e '$(BINDIR)' não foram removidos (use 'rm -rf $(OBJDIR) $(BINDIR)' para isso se necessário)."

# Declara 'all' e 'clean' como "phony targets" (alvos que não são nomes de ficheiros)
.PHONY: all clean
