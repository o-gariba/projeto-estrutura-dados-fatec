#ifndef APP_H
#define APP_H

/*
 * app.h - Camada de aplicacao e menu interativo.
 *
 * Orquestra a interacao com o usuario (via `io`) e integra a BST (`bst`)
 * com a Lista Encadeada Simples (`lista`), garantindo liberacao completa
 * de memoria ao encerrar (inclusive em EOF).
 */

#include <stdio.h>

/*
 * app_executar: executa o laco principal do menu de referencia.
 * Pre: in != NULL, out != NULL.
 * Pos: retorna 0 (EXIT_SUCCESS) apos liberar toda a memoria alocada,
 *      ou 1 (EXIT_FAILURE) se nao for possivel inicializar as estruturas.
 */
int app_executar(FILE *in, FILE *out);

#endif /* APP_H */
