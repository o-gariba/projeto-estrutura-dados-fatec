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

# Testes unitarios
TEST_SRC := $(wildcard tests/unit/test_*.c)
TEST_BIN := $(TEST_SRC:tests/unit/%.c=build/tests/%)

.PHONY: all run clean help test test-unit test-e2e memcheck sanitize

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

test-unit: $(TEST_BIN) ## Compila e executa todos os testes unitarios
	@for t in $(TEST_BIN); do \
		./$$t || exit 1; \
	done

test-e2e: $(BIN) ## Executa testes ponta-a-ponta (entrada redirecionada)
	@./tests/e2e/run_e2e.sh $(BIN)

test: test-unit test-e2e ## Roda todos os testes (unitarios e E2E)

memcheck: $(BIN) $(TEST_BIN) ## Verifica vazamentos de memoria com Valgrind
	@command -v valgrind >/dev/null 2>&1 || { echo "Erro: valgrind nao encontrado."; exit 1; }
	@for t in $(TEST_BIN); do \
		echo "[VALGRIND] $$t"; \
		valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -q ./$$t || exit 1; \
	done
	@for f in tests/e2e/casos/*.in; do \
		echo "[VALGRIND] $(BIN) < $$f"; \
		valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -q ./$(BIN) < "$$f" >/dev/null || exit 1; \
	done
	@echo "Valgrind: 0 erros e 0 vazamentos em toda a suite!"

sanitize: ## Recompila com ASan/UBSan e roda todos os testes
	@$(MAKE) clean
	@$(MAKE) CFLAGS="-std=c11 -Wall -Wextra -Wpedantic -Werror -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude" test
	@$(MAKE) clean

build/tests/%: tests/unit/%.c $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Itests/unit $< $(OBJ) -o $@

clean: ## Remove os arquivos gerados pela compilacao
	rm -rf build bin

help: ## Lista os alvos disponiveis
	@grep -E '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*## "}; {printf "  make %-10s %s\n", $$1, $$2}'
