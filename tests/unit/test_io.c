/*
 * test_io.c - Testes unitarios do modulo de leitura segura (io).
 */
#include "io.h"
#include "test.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

static FILE *criar_entrada(const char *conteudo)
{
    FILE *f = tmpfile();
    if (f != NULL && conteudo != NULL) {
        fputs(conteudo, f);
        rewind(f);
    }
    return f;
}

TEST(deve_rejeitar_texto_vazio_e_so_espacos)
{
    char buf[TAM_TEXTO];
    FILE *f = criar_entrada("\n   \t  \nRoteador Core\n");
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_ERRO_VAZIO, io_ler_texto(f, buf));
    ASSERT_EQ_INT(IO_ERRO_VAZIO, io_ler_texto(f, buf));
    ASSERT_EQ_INT(IO_OK, io_ler_texto(f, buf));
    ASSERT_EQ_STR("Roteador Core", buf);

    fclose(f);
}

TEST(deve_aceitar_texto_com_espacos_e_exatamente_79_chars)
{
    char entrada[TAM_TEXTO + 4];
    char esperada[TAM_TEXTO];
    char buf[TAM_TEXTO];
    FILE *f;

    memset(esperada, 'A', 79);
    esperada[79] = '\0';
    memcpy(entrada, esperada, 79);
    entrada[79] = '\n';
    entrada[80] = '\0';

    f = criar_entrada(entrada);
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_OK, io_ler_texto(f, buf));
    ASSERT_EQ_STR(esperada, buf);
    ASSERT_EQ_INT(79, (int)strlen(buf));

    fclose(f);
}

TEST(deve_rejeitar_texto_maior_que_79_chars_e_limpar_buffer)
{
    char linha_longa[128];
    char buf[TAM_TEXTO];
    FILE *f;

    memset(linha_longa, 'X', 80);
    strcpy(linha_longa + 80, "extra\nSwitch Sala 2\n");

    f = criar_entrada(linha_longa);
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_ERRO_LONGO, io_ler_texto(f, buf));
    /* A leitura seguinte deve vir limpa, sem resquicio do excesso */
    ASSERT_EQ_INT(IO_OK, io_ler_texto(f, buf));
    ASSERT_EQ_STR("Switch Sala 2", buf);

    fclose(f);
}

TEST(deve_aceitar_texto_79_chars_sem_newline_no_eof)
{
    char entrada[TAM_TEXTO];
    char buf[TAM_TEXTO];
    FILE *f;

    memset(entrada, 'Z', 79);
    entrada[79] = '\0';

    f = criar_entrada(entrada);
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_OK, io_ler_texto(f, buf));
    ASSERT_EQ_STR(entrada, buf);
    ASSERT_EQ_INT(IO_EOF, io_ler_texto(f, buf));

    fclose(f);
}

TEST(deve_validar_id_positivo_e_rejeitar_invalidos)
{
    int id = 0;
    FILE *f = criar_entrada(
        "\n"
        "   \n"
        "abc\n"
        "12abc\n"
        "12 34\n"
        "0\n"
        "-5\n"
        "99999999999\n"
        "99999999999999999999999999999\n"
        "  42  \n"
        "2147483647\n"
    );
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_ERRO_VAZIO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_VAZIO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_FORMATO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_FORMATO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_FORMATO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_id(f, &id));

    ASSERT_EQ_INT(IO_OK, io_ler_id(f, &id));
    ASSERT_EQ_INT(42, id);

    ASSERT_EQ_INT(IO_OK, io_ler_id(f, &id));
    ASSERT_EQ_INT(INT_MAX, id);

    ASSERT_EQ_INT(IO_EOF, io_ler_id(f, &id));

    fclose(f);
}

TEST(deve_validar_opcoes_de_menu)
{
    int op = -1;
    FILE *f = criar_entrada("-1\n8\n0\n7\n");
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_opcao(f, 0, 7, &op));
    ASSERT_EQ_INT(IO_ERRO_INTERVALO, io_ler_opcao(f, 0, 7, &op));

    ASSERT_EQ_INT(IO_OK, io_ler_opcao(f, 0, 7, &op));
    ASSERT_EQ_INT(0, op);

    ASSERT_EQ_INT(IO_OK, io_ler_opcao(f, 0, 7, &op));
    ASSERT_EQ_INT(7, op);

    fclose(f);
}

TEST(deve_validar_funcao_io_texto_valido)
{
    char grande[81];
    memset(grande, 'a', 80);
    grande[80] = '\0';

    ASSERT_EQ_INT(0, io_texto_valido(NULL));
    ASSERT_EQ_INT(0, io_texto_valido(""));
    ASSERT_EQ_INT(0, io_texto_valido("   \t\n"));
    ASSERT_EQ_INT(0, io_texto_valido(grande));
    ASSERT_EQ_INT(1, io_texto_valido("Switch L3"));
    ASSERT_EQ_INT(1, io_texto_valido("  a"));
}

TEST(deve_rejeitar_id_muito_longo_e_limpar_buffer)
{
    char entrada[128];
    int id = 0;
    FILE *f;

    memset(entrada, '1', 85);
    strcpy(entrada + 85, "\n42\n");

    f = criar_entrada(entrada);
    ASSERT_NOT_NULL(f);

    ASSERT_EQ_INT(IO_ERRO_LONGO, io_ler_id(f, &id));
    ASSERT_EQ_INT(IO_OK, io_ler_id(f, &id));
    ASSERT_EQ_INT(42, id);

    fclose(f);
}

int main(void)
{
    printf("\n=== Executando Testes do Modulo IO ===\n");

    RUN_TEST(deve_rejeitar_texto_vazio_e_so_espacos);
    RUN_TEST(deve_aceitar_texto_com_espacos_e_exatamente_79_chars);
    RUN_TEST(deve_rejeitar_texto_maior_que_79_chars_e_limpar_buffer);
    RUN_TEST(deve_aceitar_texto_79_chars_sem_newline_no_eof);
    RUN_TEST(deve_validar_id_positivo_e_rejeitar_invalidos);
    RUN_TEST(deve_validar_opcoes_de_menu);
    RUN_TEST(deve_validar_funcao_io_texto_valido);
    RUN_TEST(deve_rejeitar_id_muito_longo_e_limpar_buffer);

    return TEST_RESUMO();
}
