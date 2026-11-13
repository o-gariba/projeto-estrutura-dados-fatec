#ifndef EQUIPAMENTO_H
#define EQUIPAMENTO_H

/*
 * equipamento.h - TAD que modela um equipamento de rede.
 *
 * Invariantes garantidas por equipamento_criar():
 *  - id > 0
 *  - nome, tipo e localizacao possuem ao menos 1 caractere nao branco
 *    e comprimento maximo de 79 caracteres uteis (armazenados em char[80]).
 */

#include "io.h"
#include <stdio.h>

typedef struct {
    int id;
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char localizacao[TAM_TEXTO];
} Equipamento;

/*
 * equipamento_criar: aloca e inicializa um novo Equipamento validado.
 * Pre: id > 0; nome, tipo e localizacao validos segundo io_texto_valido().
 * Pos: retorna ponteiro alocado na heap se valido, ou NULL se algum campo
 *      for invalido ou houver falha de alocacao.
 * Complexidade: O(k), onde k e o tamanho dos textos (limitado a 79 chars).
 */
Equipamento *equipamento_criar(int id,
                               const char *nome,
                               const char *tipo,
                               const char *localizacao);

/*
 * equipamento_destruir: libera a memoria associada ao equipamento.
 * Pre: eq pode ser NULL (operacao segura / no-op).
 * Pos: memoria liberada; o ponteiro nao deve mais ser desreferenciado.
 * Complexidade: O(1).
 */
void equipamento_destruir(Equipamento *eq);

/*
 * equipamento_imprimir: exibe os dados formatados do equipamento em `out`.
 * Pre: eq != NULL, out != NULL (se algum for NULL, nao faz nada).
 * Pos: uma linha formatada e escrita em `out`.
 * Complexidade: O(1) (campos de tamanho maximo fixo 79).
 */
void equipamento_imprimir(const Equipamento *eq, FILE *out);

#endif /* EQUIPAMENTO_H */
