#ifndef TEST_H
#define TEST_H

/*
 * test.h - Mini framework de testes unitários para a disciplina de Estrutura de Dados.
 *
 * Características:
 *  - C11 puro, sem bibliotecas externas.
 *  - Usa macros com stringificação (#) para mostrar expressões que falharam.
 *  - Utiliza do { ... } while(0) para garantir expansão sintática segura de macros em C.
 *  - Retorna código de saída 0 para sucesso e 1 para falha (padrão POSIX/Makefile).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int g_testes_executados = 0;
static int g_testes_passados   = 0;
static int g_testes_falhas     = 0;
static int g_falha_atual       = 0;

/* Declaração padrão para legibilidade */
#define TEST(nome) void nome(void)

/*
 * ASSERT: avalia se uma condição lógica é verdadeira.
 * Se for falsa, exibe arquivo, linha e a expressão não satisfeita.
 */
#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("\n    [FALHA] %s:%d: expressao '%s' avaliada como falsa\n", \
               __FILE__, __LINE__, #cond); \
        g_falha_atual = 1; \
        return; \
    } \
} while (0)

/*
 * ASSERT_EQ_INT: compara dois inteiros (com cast para long long para suportar vários tipos).
 */
#define ASSERT_EQ_INT(esperado, obtido) do { \
    long long _esp = (long long)(esperado); \
    long long _obt = (long long)(obtido); \
    if (_esp != _obt) { \
        printf("\n    [FALHA] %s:%d: esperado %lld, obtido %lld (%s != %s)\n", \
               __FILE__, __LINE__, _esp, _obt, #esperado, #obtido); \
        g_falha_atual = 1; \
        return; \
    } \
} while (0)

/*
 * ASSERT_EQ_STR: compara duas strings usando strcmp de forma segura (tratando NULL).
 */
#define ASSERT_EQ_STR(esperado, obtido) do { \
    const char *_esp = (esperado); \
    const char *_obt = (obtido); \
    if (_esp == NULL || _obt == NULL) { \
        if (_esp != _obt) { \
            printf("\n    [FALHA] %s:%d: ponteiro nulo inesperado (esperado=%p, obtido=%p)\n", \
                   __FILE__, __LINE__, (const void*)_esp, (const void*)_obt); \
            g_falha_atual = 1; \
            return; \
        } \
    } else if (strcmp(_esp, _obt) != 0) { \
        printf("\n    [FALHA] %s:%d: esperado \"%s\", obtido \"%s\"\n", \
               __FILE__, __LINE__, _esp, _obt); \
        g_falha_atual = 1; \
        return; \
    } \
} while (0)

/*
 * ASSERT_NULL: garante que um ponteiro é estritamente NULL.
 */
#define ASSERT_NULL(ptr) do { \
    const void *_p = (ptr); \
    if (_p != NULL) { \
        printf("\n    [FALHA] %s:%d: esperado NULL, obtido %p (%s)\n", \
               __FILE__, __LINE__, _p, #ptr); \
        g_falha_atual = 1; \
        return; \
    } \
} while (0)

/*
 * ASSERT_NOT_NULL: garante que um ponteiro não é NULL.
 */
#define ASSERT_NOT_NULL(ptr) do { \
    const void *_p = (ptr); \
    if (_p == NULL) { \
        printf("\n    [FALHA] %s:%d: ponteiro nao deveria ser NULL (%s)\n", \
               __FILE__, __LINE__, #ptr); \
        g_falha_atual = 1; \
        return; \
    } \
} while (0)

/*
 * RUN_TEST: executa uma função de teste e registra o resultado.
 */
#define RUN_TEST(fn) do { \
    g_testes_executados++; \
    g_falha_atual = 0; \
    printf("  [TESTE] %-35s ", #fn); \
    fn(); \
    if (g_falha_atual == 0) { \
        g_testes_passados++; \
        printf("[OK]\n"); \
    } \
} while (0)

/*
 * TEST_RESUMO: imprime o placar final e retorna 0 se tudo passou, ou 1 se houve falha.
 */
#define TEST_RESUMO() ( \
    printf("\n----------------------------------------\n"), \
    printf("Total: %d | Passaram: %d | Falharam: %d\n", \
           g_testes_executados, g_testes_passados, g_testes_falhas), \
    printf("----------------------------------------\n"), \
    (g_testes_falhas == 0 ? 0 : 1) \
)

#endif /* TEST_H */
