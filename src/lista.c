#include "lista.h"

#include <stdlib.h>

Lista *lista_criar(void)
{
    Lista *lista = malloc(sizeof(*lista));
    if (lista == NULL) {
        return NULL;
    }

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

ListaStatus lista_inserir_fim(Lista *lista, int id)
{
    NoLista *novo;

    if (lista == NULL || id <= 0) {
        return LISTA_ERRO_ARGUMENTO;
    }

    novo = malloc(sizeof(*novo));
    if (novo == NULL) {
        return LISTA_ERRO_MEMORIA;
    }

    novo->id = id;
    novo->prox = NULL;

    if (lista->fim == NULL) {
        /* Lista vazia: o unico no passa a ser simultaneamente inicio e fim. */
        lista->inicio = novo;
        lista->fim = novo;
    } else {
        /* Lista populada: anexa apos `fim` e avanca `fim` em O(1). */
        lista->fim->prox = novo;
        lista->fim = novo;
    }

    lista->tamanho++;
    return LISTA_OK;
}

ListaStatus lista_remover_inicio(Lista *lista, int *id_removido)
{
    NoLista *alvo;

    if (lista == NULL) {
        return LISTA_ERRO_ARGUMENTO;
    }

    if (lista->inicio == NULL) {
        return LISTA_ERRO_VAZIA;
    }

    alvo = lista->inicio;
    if (id_removido != NULL) {
        *id_removido = alvo->id;
    }

    lista->inicio = alvo->prox;
    if (lista->inicio == NULL) {
        /* Lista ficou vazia: zera `fim` para nao virar dangling pointer. */
        lista->fim = NULL;
    }

    free(alvo);
    lista->tamanho--;
    return LISTA_OK;
}

void lista_exibir(const Lista *lista, FILE *out)
{
    const NoLista *atual;

    if (lista == NULL || out == NULL) {
        return;
    }

    if (lista->inicio == NULL) {
        fprintf(out, "(vazia)\n");
        return;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        fprintf(out, "%d", atual->id);
        if (atual->prox != NULL) {
            fprintf(out, " -> ");
        }
        atual = atual->prox;
    }
    fprintf(out, "\n");
}

int lista_vazia(const Lista *lista)
{
    return (lista == NULL || lista->tamanho == 0) ? 1 : 0;
}

size_t lista_tamanho(const Lista *lista)
{
    return (lista != NULL) ? lista->tamanho : 0;
}

void lista_destruir(Lista *lista)
{
    NoLista *atual;

    if (lista == NULL) {
        return;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        /* Guarda o proximo ponteiro antes de liberar `atual` para evitar Use-After-Free. */
        NoLista *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    free(lista);
}
