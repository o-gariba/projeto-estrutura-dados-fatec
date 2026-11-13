#ifndef LISTA_H
#define LISTA_H

/*
 * lista.h - TAD Lista Encadeada Simples de IDs inteiros positivos.
 *
 * Caracteristicas (R04, R05, R06, R07):
 *  - Armazena IDs inteiros positivos (id > 0) de forma independente da BST.
 *  - Permite IDs repetidos.
 *  - Insercao no fim em O(1) usando ponteiro `fim`.
 *  - Remocao no inicio em O(1), retornando status proprio quando vazia.
 *  - Exibicao do inicio ao fim em O(n).
 */

#include <stddef.h>
#include <stdio.h>

typedef struct NoLista {
    int id;
    struct NoLista *prox;
} NoLista;

typedef struct {
    NoLista *inicio;
    NoLista *fim;
    size_t tamanho;
} Lista;

typedef enum {
    LISTA_OK = 0,
    LISTA_ERRO_ARGUMENTO,
    LISTA_ERRO_VAZIA,
    LISTA_ERRO_MEMORIA
} ListaStatus;

/*
 * lista_criar: aloca e inicializa uma lista encadeada vazia.
 * Pre: nenhuma.
 * Pos: retorna ponteiro para Lista vazia ou NULL em caso de falha de memoria.
 * Complexidade: O(1).
 */
Lista *lista_criar(void);

/*
 * lista_inserir_fim: insere um ID positivo no final da lista.
 * Pre: lista != NULL, id > 0.
 * Pos: retorna LISTA_OK e incrementa tamanho; IDs repetidos sao aceitos.
 * Complexidade: O(1).
 */
ListaStatus lista_inserir_fim(Lista *lista, int id);

/*
 * lista_remover_inicio: remove o primeiro elemento da lista.
 * Pre: lista != NULL (id_removido pode ser NULL se o valor nao interessar).
 * Pos: se nao vazia, remove o primeiro no, grava o ID em *id_removido (se != NULL),
 *      decrementa tamanho e retorna LISTA_OK. Se vazia, retorna LISTA_ERRO_VAZIA
 *      sem modificar a lista.
 * Complexidade: O(1).
 */
ListaStatus lista_remover_inicio(Lista *lista, int *id_removido);

/*
 * lista_exibir: imprime os IDs do inicio ao fim em `out`.
 * Pre: lista != NULL, out != NULL.
 * Pos: exibe os IDs na ordem de insercao (ou "(vazia)" se nao houver elementos).
 * Complexidade: O(n).
 */
void lista_exibir(const Lista *lista, FILE *out);

/*
 * lista_vazia: retorna 1 se a lista estiver vazia (ou NULL), 0 caso contrario.
 * Complexidade: O(1).
 */
int lista_vazia(const Lista *lista);

/*
 * lista_tamanho: retorna a quantidade de IDs armazenados na lista.
 * Complexidade: O(1).
 */
size_t lista_tamanho(const Lista *lista);

/*
 * lista_destruir: libera todos os nos da lista e o descritor Lista.
 * Pre: lista pode ser NULL (no-op).
 * Pos: toda a memoria alocada e liberada.
 * Complexidade: O(n).
 */
void lista_destruir(Lista *lista);

#endif /* LISTA_H */
