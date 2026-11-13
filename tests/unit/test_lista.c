/*
 * test_lista.c - Testes unitarios da Lista Encadeada Simples (lista).
 */
#include "lista.h"
#include "test.h"

#include <stdio.h>

TEST(deve_criar_lista_vazia_e_tratar_remocao_vazia)
{
    Lista *lista = lista_criar();
    int id = -1;

    ASSERT_NOT_NULL(lista);
    ASSERT_EQ_INT(1, lista_vazia(lista));
    ASSERT_EQ_INT(0, (int)lista_tamanho(lista));
    ASSERT_NULL(lista->inicio);
    ASSERT_NULL(lista->fim);

    /* Remocao em lista vazia nao deve falhar nem alterar o estado */
    ASSERT_EQ_INT(LISTA_ERRO_VAZIA, lista_remover_inicio(lista, &id));
    ASSERT_EQ_INT(-1, id);
    ASSERT_EQ_INT(1, lista_vazia(lista));

    lista_destruir(lista);
    lista_destruir(NULL);
}

TEST(deve_rejeitar_ids_nao_positivos_e_permitir_repetidos)
{
    Lista *lista = lista_criar();
    FILE *out = tmpfile();
    char buf[128];

    ASSERT_NOT_NULL(lista);
    ASSERT_NOT_NULL(out);

    ASSERT_EQ_INT(LISTA_ERRO_ARGUMENTO, lista_inserir_fim(lista, 0));
    ASSERT_EQ_INT(LISTA_ERRO_ARGUMENTO, lista_inserir_fim(lista, -10));
    ASSERT_EQ_INT(0, (int)lista_tamanho(lista));

    /* IDs repetidos sao permitidos e preservam a ordem de insercao no fim */
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 42));
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 7));
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 42));
    ASSERT_EQ_INT(3, (int)lista_tamanho(lista));

    lista_exibir(lista, out);
    rewind(out);
    ASSERT_NOT_NULL(fgets(buf, (int)sizeof(buf), out));
    ASSERT_EQ_STR("42 -> 7 -> 42\n", buf);

    fclose(out);
    lista_destruir(lista);
}

TEST(deve_inserir_fim_remover_inicio_e_reinserir_apos_esvaziar)
{
    Lista *lista = lista_criar();
    int id = 0;

    ASSERT_NOT_NULL(lista);

    /* Insercao e remocao de um unico elemento (testa reset de `fim`) */
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 100));
    ASSERT_EQ_INT(1, (int)lista_tamanho(lista));
    ASSERT_EQ_INT(LISTA_OK, lista_remover_inicio(lista, &id));
    ASSERT_EQ_INT(100, id);
    ASSERT_EQ_INT(1, lista_vazia(lista));
    ASSERT_NULL(lista->inicio);
    ASSERT_NULL(lista->fim);

    /* Reinsercao apos esvaziar completamente */
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 10));
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 20));
    ASSERT_EQ_INT(LISTA_OK, lista_inserir_fim(lista, 30));

    ASSERT_EQ_INT(LISTA_OK, lista_remover_inicio(lista, &id));
    ASSERT_EQ_INT(10, id);
    ASSERT_EQ_INT(LISTA_OK, lista_remover_inicio(lista, &id));
    ASSERT_EQ_INT(20, id);
    ASSERT_EQ_INT(1, (int)lista_tamanho(lista));
    ASSERT_EQ_INT(30, lista->inicio->id);
    ASSERT_EQ_INT(30, lista->fim->id);

    /* Destruicao de lista ainda populada nao deve vazar memoria */
    lista_destruir(lista);
}

int main(void)
{
    printf("\n=== Executando Testes da Lista Encadeada ===\n");

    RUN_TEST(deve_criar_lista_vazia_e_tratar_remocao_vazia);
    RUN_TEST(deve_rejeitar_ids_nao_positivos_e_permitir_repetidos);
    RUN_TEST(deve_inserir_fim_remover_inicio_e_reinserir_apos_esvaziar);

    return TEST_RESUMO();
}
