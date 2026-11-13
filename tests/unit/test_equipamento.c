/*
 * test_equipamento.c - Testes unitarios do TAD Equipamento.
 */
#include "equipamento.h"
#include "test.h"

#include <stdio.h>
#include <string.h>

TEST(deve_criar_equipamento_valido_e_imprimir)
{
    Equipamento *eq = equipamento_criar(10, "Core Router 01", "Roteador", "Rack A1");
    FILE *out;
    char linha[256];

    ASSERT_NOT_NULL(eq);
    ASSERT_EQ_INT(10, eq->id);
    ASSERT_EQ_STR("Core Router 01", eq->nome);
    ASSERT_EQ_STR("Roteador", eq->tipo);
    ASSERT_EQ_STR("Rack A1", eq->localizacao);

    out = tmpfile();
    ASSERT_NOT_NULL(out);
    equipamento_imprimir(eq, out);
    rewind(out);
    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT_EQ_STR("ID: 10 | Nome: Core Router 01 | Tipo: Roteador | Localizacao: Rack A1\n",
                  linha);

    fclose(out);
    equipamento_destruir(eq);
}

TEST(deve_rejeitar_id_invalido)
{
    ASSERT_NULL(equipamento_criar(0, "Switch 01", "Switch", "Sala 1"));
    ASSERT_NULL(equipamento_criar(-7, "Switch 01", "Switch", "Sala 1"));
}

TEST(deve_rejeitar_campos_textuais_invalidos)
{
    char longo[81];
    memset(longo, 'B', 80);
    longo[80] = '\0';

    /* Ponteiros NULL */
    ASSERT_NULL(equipamento_criar(1, NULL, "Switch", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", NULL, "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "Switch", NULL));

    /* Strings vazias */
    ASSERT_NULL(equipamento_criar(1, "", "Switch", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "Switch", ""));

    /* Somente espacos */
    ASSERT_NULL(equipamento_criar(1, "   \t ", "Switch", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "   ", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "Switch", " \n "));

    /* Mais de 79 caracteres */
    ASSERT_NULL(equipamento_criar(1, longo, "Switch", "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", longo, "Sala 1"));
    ASSERT_NULL(equipamento_criar(1, "SW-01", "Switch", longo));
}

TEST(deve_aceitar_exatamente_79_caracteres_e_destruir_null)
{
    char limite[80];
    Equipamento *eq;

    memset(limite, 'R', 79);
    limite[79] = '\0';

    eq = equipamento_criar(1, limite, "Roteador", "CPD");
    ASSERT_NOT_NULL(eq);
    ASSERT_EQ_INT(79, (int)strlen(eq->nome));

    equipamento_destruir(eq);
    /* Nao deve falhar com NULL */
    equipamento_destruir(NULL);
    equipamento_imprimir(NULL, stdout);
}

int main(void)
{
    printf("\n=== Executando Testes do TAD Equipamento ===\n");

    RUN_TEST(deve_criar_equipamento_valido_e_imprimir);
    RUN_TEST(deve_rejeitar_id_invalido);
    RUN_TEST(deve_rejeitar_campos_textuais_invalidos);
    RUN_TEST(deve_aceitar_exatamente_79_caracteres_e_destruir_null);

    return TEST_RESUMO();
}
