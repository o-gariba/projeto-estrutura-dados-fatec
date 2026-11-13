#ifndef BST_H
#define BST_H

/*
 * bst.h - TAD Arvore Binaria de Busca (BST) indexada por id de Equipamento.
 *
 * Invariantes da BST:
 *  - Para todo no N: todos os IDs na subarvore esquerda sao < N->dado->id,
 *    e todos os IDs na subarvore direita sao > N->dado->id.
 *  - Nao sao permitidos IDs duplicados nem IDs <= 0.
 *  - A arvore detem a posse (ownership) dos Equipamento* inseridos com sucesso,
 *    liberando-os em bst_remover() e bst_destruir().
 */

#include "equipamento.h"
#include <stddef.h>
#include <stdio.h>

typedef struct NoBST {
    Equipamento *dado;
    struct NoBST *esq;
    struct NoBST *dir;
} NoBST;

typedef struct {
    NoBST *raiz;
    size_t tamanho;
} BST;

typedef enum {
    BST_OK = 0,
    BST_ERRO_ARGUMENTO,
    BST_ERRO_ID_DUPLICADO,
    BST_ERRO_NAO_ENCONTRADO,
    BST_ERRO_MEMORIA
} BstStatus;

/*
 * bst_criar: aloca e inicializa uma BST vazia.
 * Pre: nenhuma.
 * Pos: retorna ponteiro para BST vazia ou NULL em caso de falha de memoria.
 * Complexidade: O(1).
 */
BST *bst_criar(void);

/*
 * bst_inserir: insere um Equipamento na BST mantendo a propriedade de ordenacao.
 * Pre: arv != NULL, eq != NULL, eq->id > 0.
 * Pos: se retornar BST_OK, a BST passa a deter a posse de `eq` e tamanho aumenta em 1.
 *      Se retornar erro (ex.: BST_ERRO_ID_DUPLICADO), `eq` NAO e inserido nem liberado;
 *      o chamador mantem a posse de `eq`.
 * Complexidade: O(h), sendo O(log n) no caso medio e O(n) no pior caso.
 */
BstStatus bst_inserir(BST *arv, Equipamento *eq);

/*
 * bst_buscar: localiza um equipamento pelo ID sem modificar a arvore.
 * Pre: arv != NULL.
 * Pos: retorna ponteiro somente leitura para o Equipamento ou NULL se nao existir.
 * Complexidade: O(h) em tempo, O(1) em espaco auxiliar.
 */
const Equipamento *bst_buscar(const BST *arv, int id);

/*
 * bst_remover: remove o no com o ID informado (tratando 0, 1 ou 2 filhos)
 * e libera o Equipamento correspondente.
 * Pre: arv != NULL.
 * Pos: se encontrado, retorna BST_OK e decrementa tamanho; se nao, retorna
 *      BST_ERRO_NAO_ENCONTRADO.
 * Complexidade: O(h).
 */
BstStatus bst_remover(BST *arv, int id);

/*
 * bst_listar_em_ordem: percorre a arvore em-ordem (esq -> raiz -> dir),
 * imprimindo os equipamentos em ordem crescente de ID.
 * Pre: arv != NULL, out != NULL.
 * Pos: registros escritos em `out` em ordem crescente de ID.
 * Complexidade: O(n).
 */
void bst_listar_em_ordem(const BST *arv, FILE *out);

/*
 * bst_tamanho: retorna a quantidade de registros armazenados na BST.
 * Complexidade: O(1).
 */
size_t bst_tamanho(const BST *arv);

/*
 * bst_vazia: retorna 1 se a BST estiver vazia (ou NULL), 0 caso contrario.
 * Complexidade: O(1).
 */
int bst_vazia(const BST *arv);

/*
 * bst_destruir: libera em pos-ordem todos os nos, equipamentos e o descritor BST.
 * Pre: arv pode ser NULL (no-op).
 * Pos: toda a memoria da arvore e liberada.
 * Complexidade: O(n).
 */
void bst_destruir(BST *arv);

#endif /* BST_H */
