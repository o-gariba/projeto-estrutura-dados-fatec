# =============================================================================
#  Makefile - Rede de Equipamentos e Conexoes
#  Digite `make help` para ver todos os alvos disponiveis.
# =============================================================================

# Compilador (pode ser trocado na linha de comando: `make CC=clang`).
# Obs.: usamos "=" e nao "?=" porque o make ja define CC=cc por padrao,
# entao "?=" nunca teria efeito.
CC      = gcc

# Flags de compilacao (explicadas no README, secao "Entendendo os comandos")
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -g
CFLAGS  += -Iinclude

# Todos os .c de src/, exceto main.c (os objetos serao reutilizados pelos testes)
SRC     := $(filter-out src/main.c,$(wildcard src/*.c))
OBJ     := $(SRC:src/%.c=build/%.o)
BIN     := bin/rede

.PHONY: all run clean help

all: $(BIN) ## Compila a aplicacao em bin/rede

$(BIN): $(OBJ) build/main.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# Regra generica: cada src/X.c vira build/X.o
build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN) ## Compila (se necessario) e executa a aplicacao
	./$(BIN)

clean: ## Remove os arquivos gerados pela compilacao
	rm -rf build bin

help: ## Lista os alvos disponiveis
	@grep -E '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*## "}; {printf "  make %-10s %s\n", $$1, $$2}'
