#include "io.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void descartar_resto_linha(FILE *in)
{
    int ch;
    while ((ch = fgetc(in)) != '\n' && ch != EOF) {
    }
}

IoStatus io_ler_linha(FILE *in, char buf[TAM_TEXTO])
{
    /*
     * Buffer interno com 1 byte extra (81 bytes) para conseguir ler ate
     * 79 caracteres uteis + '\n' + '\0' em uma unica chamada a fgets.
     */
    char tmp[TAM_TEXTO + 1];
    size_t len;
    char *pos_nl;

    if (in == NULL || buf == NULL) {
        return IO_ERRO_FORMATO;
    }

    if (fgets(tmp, (int)sizeof(tmp), in) == NULL) {
        buf[0] = '\0';
        return IO_EOF;
    }

    pos_nl = strchr(tmp, '\n');
    if (pos_nl != NULL) {
        *pos_nl = '\0';
        len = strlen(tmp);
        if (len > 0 && tmp[len - 1] == '\r') {
            tmp[len - 1] = '\0';
        }
        memcpy(buf, tmp, strlen(tmp) + 1);
        return IO_OK;
    }

    /*
     * Nao encontrou '\n'. Duas possibilidades:
     * 1) A linha tem exatamente <= 79 chars e terminou em EOF (sem '\n').
     * 2) A linha tem 80+ caracteres e excedeu o limite.
     */
    len = strlen(tmp);
    if (len < TAM_TEXTO) {
        /* Menos de 80 chars sem '\n' significa que o fluxo chegou ao EOF. */
        if (len > 0 && tmp[len - 1] == '\r') {
            tmp[len - 1] = '\0';
        }
        memcpy(buf, tmp, strlen(tmp) + 1);
        return IO_OK;
    }

    /* Leu 80 caracteres sem achar '\n': ultrapassou os 79 uteis permitidos. */
    descartar_resto_linha(in);
    buf[0] = '\0';
    return IO_ERRO_LONGO;
}

int io_texto_valido(const char *s)
{
    size_t i;

    if (s == NULL || s[0] == '\0') {
        return 0;
    }

    for (i = 0; s[i] != '\0'; i++) {
        if (i >= TAM_TEXTO - 1) {
            return 0;
        }
        if (!isspace((unsigned char)s[i])) {
            /* Ainda verifica se o restante cabe no limite de 79 caracteres. */
            return strlen(s) <= TAM_TEXTO - 1;
        }
    }

    return 0;
}

IoStatus io_ler_texto(FILE *in, char destino[TAM_TEXTO])
{
    IoStatus st = io_ler_linha(in, destino);
    if (st != IO_OK) {
        return st;
    }

    if (!io_texto_valido(destino)) {
        destino[0] = '\0';
        return IO_ERRO_VAZIO;
    }

    return IO_OK;
}

IoStatus io_ler_inteiro_intervalo(FILE *in, int min, int max, int *destino)
{
    char buf[TAM_TEXTO];
    char *endptr;
    const char *p;
    long valor;
    IoStatus st;

    if (in == NULL || destino == NULL || min > max) {
        return IO_ERRO_FORMATO;
    }

    st = io_ler_linha(in, buf);
    if (st != IO_OK) {
        return st;
    }

    if (!io_texto_valido(buf)) {
        return IO_ERRO_VAZIO;
    }

    errno = 0;
    valor = strtol(buf, &endptr, 10);

    if (endptr == buf) {
        return IO_ERRO_FORMATO;
    }

    /* Permite apenas espacos em branco apos o numero. */
    p = endptr;
    while (*p != '\0') {
        if (!isspace((unsigned char)*p)) {
            return IO_ERRO_FORMATO;
        }
        p++;
    }

    if (errno == ERANGE || valor < (long)INT_MIN || valor > (long)INT_MAX) {
        return IO_ERRO_INTERVALO;
    }

    if (valor < (long)min || valor > (long)max) {
        return IO_ERRO_INTERVALO;
    }

    *destino = (int)valor;
    return IO_OK;
}

IoStatus io_ler_id(FILE *in, int *destino)
{
    return io_ler_inteiro_intervalo(in, 1, INT_MAX, destino);
}

IoStatus io_ler_opcao(FILE *in, int min, int max, int *destino)
{
    return io_ler_inteiro_intervalo(in, min, max, destino);
}

const char *io_status_mensagem(IoStatus status)
{
    switch (status) {
        case IO_OK:
            return "Entrada valida.";
        case IO_ERRO_VAZIO:
            return "Entrada vazia ou apenas espacos em branco.";
        case IO_ERRO_LONGO:
            return "Entrada excede o limite de 79 caracteres.";
        case IO_ERRO_FORMATO:
            return "Formato invalido. Informe um numero inteiro valido.";
        case IO_ERRO_INTERVALO:
            return "Valor fora do intervalo permitido.";
        case IO_EOF:
            return "Fim da entrada alcancado.";
        default:
            return "Erro desconhecido de entrada.";
    }
}
