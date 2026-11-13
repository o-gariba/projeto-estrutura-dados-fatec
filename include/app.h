#ifndef APP_H
#define APP_H

/*
 * app.h - Camada de aplicacao e menu interativo.
 *
 * Orquestra a interacao com o usuario (via `io`) e integra a BST (`bst`),
 * a Lista Encadeada Simples (`lista`) e o gerador de dados (`faker`),
 * garantindo liberacao completa de memoria ao encerrar (inclusive em EOF).
 */

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t semente;
    int usar_semente_fixa; /* 0 = usa relogio (time), 1 = usa `semente` */
    int modo_interativo;   /* -1 = auto (TTY), 0 = lote, 1 = pausas + limpar tela */
} AppConfig;

/*
 * app_executar: executa o laco principal com configuracao padrao
 * (semente baseada no relogio e deteccao automatica de terminal interativo).
 * Pre: in != NULL, out != NULL.
 * Pos: retorna 0 (EXIT_SUCCESS) apos liberar toda a memoria alocada,
 *      ou 1 (EXIT_FAILURE) se nao for possivel inicializar as estruturas.
 */
int app_executar(FILE *in, FILE *out);

/*
 * app_executar_config: executa o laco principal com configuracao explicita.
 * Pre: in != NULL, out != NULL (cfg pode ser NULL para usar o padrao).
 * Pos: retorna 0 (EXIT_SUCCESS) apos liberar toda a memoria alocada.
 */
int app_executar_config(FILE *in, FILE *out, const AppConfig *cfg);

#endif /* APP_H */
