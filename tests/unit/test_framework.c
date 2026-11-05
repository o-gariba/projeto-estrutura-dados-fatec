/*
 * test_framework.c - Validação de fumaça das macros do mini framework de testes.
 */
#include "test.h"

TEST(deve_passar_em_assert_simples)
{
    ASSERT(1 == 1);
    ASSERT(10 > 2);
}

TEST(deve_comparar_inteiros)
{
    int a = 42;
    int b = 42;
    ASSERT_EQ_INT(42, a);
    ASSERT_EQ_INT(a, b);
}

TEST(deve_comparar_strings)
{
    const char *s1 = "Fatec";
    const char *s2 = "Fatec";
    ASSERT_EQ_STR("Fatec", s1);
    ASSERT_EQ_STR(s1, s2);
}

TEST(deve_tratar_ponteiros_nulos)
{
    void *ptr_nulo = NULL;
    int valor = 10;
    void *ptr_valido = &valor;

    ASSERT_NULL(ptr_nulo);
    ASSERT_NOT_NULL(ptr_valido);
}

int main(void)
{
    printf("\n=== Executando Testes do Framework ===\n");

    RUN_TEST(deve_passar_em_assert_simples);
    RUN_TEST(deve_comparar_inteiros);
    RUN_TEST(deve_comparar_strings);
    RUN_TEST(deve_tratar_ponteiros_nulos);

    return TEST_RESUMO();
}
