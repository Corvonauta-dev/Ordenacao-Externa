CC       := gcc
CFLAGS   := -std=c11 -O2 -Iinclude
SRCDIR   := src
OBJDIR   := obj
BINDIR   := bin

# Todos os módulos .c, incluindo quicksort.c
SOURCES := $(wildcard $(SRCDIR)/*.c)
# Converte src/arquivo.c → obj/arquivo.o
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
TARGET  := $(BINDIR)/ordenacao-externa

all: $(TARGET)

# Linka todos os objetos em um executável
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compila cada .c em .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Cria pastas obj/ e bin/ se não existirem
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Limpeza (remove objetos e executável)
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

.PHONY: all clean
