/*
 * test_faker.c - Testes unitarios do modulo Faker e do modo interativo de UI.
 */
#include "app.h"
#include "faker.h"
#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST(deve_inicializar_com_determinismo_e_tratar_semente_zero)
{
    Faker f1;
    Faker f2;
    Faker f3;
    int i;

    faker_inicializar(&f1, 42u);
    faker_inicializar(&f2, 42u);
    faker_inicializar(&f3, 0u);

    ASSERT(f3.estado != 0u);
    for (i = 0; i < 10; i++) {
        ASSERT_EQ_INT((int)faker_proximo_u32(&f1),
                      (int)faker_proximo_u32(&f2));
    }

    faker_inicializar(NULL, 10u);
    ASSERT_EQ_INT(0, (int)faker_proximo_u32(NULL));
}

TEST(deve_gerar_equipamento_valido_e_rejeitar_parametros_invalidos)
{
    Faker fk;
    Equipamento *eq;

    faker_inicializar(&fk, 123u);
    ASSERT_NULL(faker_gerar_equipamento(NULL, 1));
    ASSERT_NULL(faker_gerar_equipamento(&fk, 0));
    ASSERT_NULL(faker_gerar_equipamento(&fk, -4));

    eq = faker_gerar_equipamento(&fk, 77);
    ASSERT_NOT_NULL(eq);
    ASSERT_EQ_INT(77, eq->id);
    ASSERT_EQ_INT(1, io_texto_valido(eq->nome));
    ASSERT_EQ_INT(1, io_texto_valido(eq->tipo));
    ASSERT_EQ_INT(1, io_texto_valido(eq->localizacao));

    equipamento_destruir(eq);
}

TEST(deve_popular_bst_sem_colisoes_e_preservar_existentes)
{
    BST *arv = bst_criar();
    Faker fk;
    Equipamento *manual;
    const Equipamento *achado;
    size_t inseridos;

    ASSERT_NOT_NULL(arv);
    faker_inicializar(&fk, 2026u);

    /* Argumentos invalidos nao inserem nada */
    ASSERT_EQ_INT(0, (int)faker_popular_bst(NULL, arv, 5));
    ASSERT_EQ_INT(0, (int)faker_popular_bst(&fk, NULL, 5));
    ASSERT_EQ_INT(0, (int)faker_popular_bst(&fk, arv, 0));
    ASSERT_EQ_INT(0, (int)faker_popular_bst(&fk, arv, FAKER_MAX_QTD + 1));

    /* Pre-popula um registro manual que deve permanecer intacto */
    manual = equipamento_criar(10, "Core-Manual", "Roteador", "CPD");
    ASSERT_NOT_NULL(manual);
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, manual));

    inseridos = faker_popular_bst(&fk, arv, 50);
    ASSERT_EQ_INT(50, (int)inseridos);
    ASSERT_EQ_INT(51, (int)bst_tamanho(arv));

    achado = bst_buscar(arv, 10);
    ASSERT_NOT_NULL(achado);
    ASSERT_EQ_STR("Core-Manual", achado->nome);

    bst_destruir(arv);
}

TEST(deve_pausar_limpar_tela_e_encerrar_em_modo_interativo)
{
    FILE *in = tmpfile();
    FILE *out = tmpfile();
    AppConfig cfg;
    char buf[4096];
    size_t lidos;
    int rc;

    ASSERT_NOT_NULL(in);
    ASSERT_NOT_NULL(out);

    /*
     * Simula:
     * 1) Opcao 6 (Faker) -> quantidade 3 -> ENTER da pausa
     * 2) Opcao 5 (Arvore) -> EOF na pausa (encerra sem vazamentos)
     */
    fputs("6\n3\n\n5\n", in);
    rewind(in);

    cfg.semente = 42u;
    cfg.usar_semente_fixa = 1;
    cfg.modo_interativo = 1;

    rc = app_executar_config(in, out, &cfg);
    ASSERT_EQ_INT(EXIT_SUCCESS, rc);

    rewind(out);
    lidos = fread(buf, 1, sizeof(buf) - 1, out);
    buf[lidos] = '\0';

    ASSERT(strstr(buf, "\033[2J\033[H") != NULL);
    ASSERT(strstr(buf, "Foram gerados e inseridos 3 registros na BST") != NULL);
    ASSERT(strstr(buf, "Pressione ENTER para continuar...") != NULL);
    ASSERT(strstr(buf, "Fim da entrada. Encerrando sistema...") != NULL);

    fclose(in);
    fclose(out);
}

int main(void)
{
    printf("\n=== Executando Testes do Modulo Faker e UX ===\n");

    RUN_TEST(deve_inicializar_com_determinismo_e_tratar_semente_zero);
    RUN_TEST(deve_gerar_equipamento_valido_e_rejeitar_parametros_invalidos);
    RUN_TEST(deve_popular_bst_sem_colisoes_e_preservar_existentes);
    RUN_TEST(deve_pausar_limpar_tela_e_encerrar_em_modo_interativo);

    return TEST_RESUMO();
}
