#include "bst.h"

#include <stdlib.h>

/*
 * Padrao recursivo com retorno de subarvore: cada chamada devolve a raiz
 * (possivelmente nova ou modificada) da subarvore atual, permitindo que o pai
 * religue `no->esq` ou `no->dir` sem precisar usar ponteiro duplo (NoBST **).
 */
static NoBST *inserir_recursivo(NoBST *no, Equipamento *eq, BstStatus *status)
{
    if (no == NULL) {
        /* Aloca o NoBST apenas quando encontra a folha vazia (evita malloc inutil em duplicatas). */
        NoBST *novo = malloc(sizeof(*novo));
        if (novo == NULL) {
            *status = BST_ERRO_MEMORIA;
            return NULL;
        }
        novo->dado = eq;
        novo->esq = NULL;
        novo->dir = NULL;
        *status = BST_OK;
        return novo;
    }

    if (eq->id == no->dado->id) {
        *status = BST_ERRO_ID_DUPLICADO;
        return no;
    }

    if (eq->id < no->dado->id) {
        no->esq = inserir_recursivo(no->esq, eq, status);
    } else {
        no->dir = inserir_recursivo(no->dir, eq, status);
    }

    return no;
}

static NoBST *remover_recursivo(NoBST *no, int id, int *removido)
{
    NoBST *pai_suc;
    NoBST *suc;

    if (no == NULL) {
        return NULL;
    }

    if (id < no->dado->id) {
        no->esq = remover_recursivo(no->esq, id, removido);
        return no;
    }

    if (id > no->dado->id) {
        no->dir = remover_recursivo(no->dir, id, removido);
        return no;
    }

    /* Encontrou o no a remover */
    *removido = 1;

    /* Caso 1: no folha (0 filhos) */
    if (no->esq == NULL && no->dir == NULL) {
        equipamento_destruir(no->dado);
        free(no);
        return NULL;
    }

    /* Caso 2a: apenas filho a direita */
    if (no->esq == NULL) {
        NoBST *filho = no->dir;
        equipamento_destruir(no->dado);
        free(no);
        return filho;
    }

    /* Caso 2b: apenas filho a esquerda */
    if (no->dir == NULL) {
        NoBST *filho = no->esq;
        equipamento_destruir(no->dado);
        free(no);
        return filho;
    }

    /*
     * Caso 3: 2 filhos.
     * Localiza o sucessor em-ordem (menor ID da subarvore direita, que por
     * definicao nao possui filho esquerdo: suc->esq == NULL).
     * Transfere o ponteiro Equipamento* do sucessor para o no atual e libera
     * somente o NoBST estrutural do sucessor (evitando double free).
     *
     *   (a) pai_suc == no:             (b) pai_suc != no:
     *          no                             no
     *           \                              \
     *           suc (pai_suc->dir = suc->dir)  ... -> pai_suc
     *             \                                    /
     *           suc->dir                             suc (pai_suc->esq = suc->dir)
     *                                                  \
     *                                                suc->dir
     */
    pai_suc = no;
    suc = no->dir;
    while (suc->esq != NULL) {
        pai_suc = suc;
        suc = suc->esq;
    }

    equipamento_destruir(no->dado);
    no->dado = suc->dado;

    if (pai_suc == no) {
        pai_suc->dir = suc->dir;
    } else {
        pai_suc->esq = suc->dir;
    }

    free(suc);
    return no;
}

static void listar_em_ordem_recursivo(const NoBST *no, FILE *out)
{
    if (no == NULL) {
        return;
    }

    listar_em_ordem_recursivo(no->esq, out);
    equipamento_imprimir(no->dado, out);
    listar_em_ordem_recursivo(no->dir, out);
}

/*
 * Percurso pos-ordem (esq -> dir -> raiz) e obrigatorio na destruicao:
 * liberar `no` antes dos filhos (pre-ordem) causaria Use-After-Free ao ler
 * `no->esq` ou `no->dir`.
 */
static void destruir_recursivo(NoBST *no)
{
    if (no == NULL) {
        return;
    }

    destruir_recursivo(no->esq);
    destruir_recursivo(no->dir);
    equipamento_destruir(no->dado);
    free(no);
}

BST *bst_criar(void)
{
    BST *arv = malloc(sizeof(*arv));
    if (arv == NULL) {
        return NULL;
    }

    arv->raiz = NULL;
    arv->tamanho = 0;
    return arv;
}

BstStatus bst_inserir(BST *arv, Equipamento *eq)
{
    BstStatus status = BST_OK;

    if (arv == NULL || eq == NULL || eq->id <= 0) {
        return BST_ERRO_ARGUMENTO;
    }

    arv->raiz = inserir_recursivo(arv->raiz, eq, &status);
    if (status == BST_OK) {
        arv->tamanho++;
    }

    return status;
}

/* Busca iterativa: nao consome pilha de chamadas, mantendo espaco auxiliar O(1). */
const Equipamento *bst_buscar(const BST *arv, int id)
{
    const NoBST *atual;

    if (arv == NULL || id <= 0) {
        return NULL;
    }

    atual = arv->raiz;
    while (atual != NULL) {
        if (id == atual->dado->id) {
            return atual->dado;
        }
        atual = (id < atual->dado->id) ? atual->esq : atual->dir;
    }

    return NULL;
}

BstStatus bst_remover(BST *arv, int id)
{
    int removido = 0;

    if (arv == NULL || id <= 0) {
        return BST_ERRO_ARGUMENTO;
    }

    arv->raiz = remover_recursivo(arv->raiz, id, &removido);
    if (!removido) {
        return BST_ERRO_NAO_ENCONTRADO;
    }

    arv->tamanho--;
    return BST_OK;
}

void bst_listar_em_ordem(const BST *arv, FILE *out)
{
    if (arv == NULL || out == NULL) {
        return;
    }

    listar_em_ordem_recursivo(arv->raiz, out);
}

size_t bst_tamanho(const BST *arv)
{
    return (arv != NULL) ? arv->tamanho : 0;
}

int bst_vazia(const BST *arv)
{
    return (arv == NULL || arv->tamanho == 0) ? 1 : 0;
}

void bst_destruir(BST *arv)
{
    if (arv == NULL) {
        return;
    }

    destruir_recursivo(arv->raiz);
    free(arv);
}
