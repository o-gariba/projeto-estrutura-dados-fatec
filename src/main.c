/*
 * main.c - Ponto de entrada da aplicacao.
 *
 * Suporta flags opcionais de linha de comando:
 *   --seed <N>       Define semente deterministica para o Faker.
 *   --interativo     Forca pausas e limpeza de tela mesmo fora de TTY.
 *   --batch          Desativa pausas e limpeza de tela (modo redirecionamento).
 */
#include "app.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int analisar_seed(const char *arg, uint32_t *semente)
{
    char *endptr = NULL;
    unsigned long valor;

    if (arg == NULL || arg[0] == '\0') {
        return 0;
    }

    errno = 0;
    valor = strtoul(arg, &endptr, 10);
    if (errno == ERANGE || endptr == arg || *endptr != '\0') {
        return 0;
    }

    *semente = (uint32_t)valor;
    return 1;
}

int main(int argc, char **argv)
{
    AppConfig cfg;
    int i;

    cfg.semente = 0u;
    cfg.usar_semente_fixa = 0;
    cfg.modo_interativo = -1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc || !analisar_seed(argv[i + 1], &cfg.semente)) {
                fprintf(stderr, "Erro: uso correto '--seed <numero_nao_negativo>'.\n");
                return EXIT_FAILURE;
            }
            cfg.usar_semente_fixa = 1;
            i++;
        } else if (strcmp(argv[i], "--interativo") == 0) {
            cfg.modo_interativo = 1;
        } else if (strcmp(argv[i], "--batch") == 0) {
            cfg.modo_interativo = 0;
        } else {
            fprintf(stderr, "Erro: argumento desconhecido '%s'.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    return app_executar_config(stdin, stdout, &cfg);
}
