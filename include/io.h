#ifndef IO_H
#define IO_H

/*
 * io.h - Modulo de leitura segura e validacao de entradas.
 *
 * Responsavel por ler linhas de texto sem estourar buffers, descartar excessos
 * remanescentes no fluxo de entrada e converter inteiros com verificacao
 * estrita de formato e intervalo.
 */

#include <stdio.h>

/* 79 caracteres uteis + '\0' */
#define TAM_TEXTO 80

typedef enum {
    IO_OK = 0,
    IO_ERRO_VAZIO,
    IO_ERRO_LONGO,
    IO_ERRO_FORMATO,
    IO_ERRO_INTERVALO,
    IO_EOF
} IoStatus;

/*
 * io_ler_linha: le uma linha de `in` para `buf` (ate TAM_TEXTO - 1 caracteres).
 * Pre: in != NULL, buf != NULL (capacidade >= TAM_TEXTO).
 * Pos: se retornar IO_OK, `buf` contem a linha sem '\n'/'\r' e terminada em '\0'.
 *      Se a linha exceder 79 caracteres uteis, consome o restante ate '\n'/EOF
 *      e retorna IO_ERRO_LONGO (deixando o fluxo limpo para a proxima leitura).
 * Complexidade: O(k), onde k e o tamanho da linha lida.
 */
IoStatus io_ler_linha(FILE *in, char buf[TAM_TEXTO]);

/*
 * io_texto_valido: verifica se a string possui ao menos 1 caractere nao branco
 * e cabe no limite de TAM_TEXTO - 1 caracteres uteis.
 * Pre: s != NULL (se NULL, retorna 0).
 * Pos: retorna 1 se valida, 0 caso contrario.
 * Complexidade: O(k).
 */
int io_texto_valido(const char *s);

/*
 * io_ler_texto: le uma linha de `in` e valida que nao esta vazia/so espacos
 * e nao ultrapassa 79 caracteres uteis.
 * Pre: in != NULL, destino != NULL (capacidade >= TAM_TEXTO).
 * Pos: se retornar IO_OK, `destino` recebe o texto valido.
 * Complexidade: O(k).
 */
IoStatus io_ler_texto(FILE *in, char destino[TAM_TEXTO]);

/*
 * io_ler_inteiro_intervalo: le uma linha e converte para int dentro de [min, max].
 * Pre: in != NULL, destino != NULL, min <= max.
 * Pos: se retornar IO_OK, *destino recebe o valor convertido.
 *      Rejeita vazio, texto nao numerico, sufixos invalidos e overflow de int.
 * Complexidade: O(k).
 */
IoStatus io_ler_inteiro_intervalo(FILE *in, int min, int max, int *destino);

/*
 * io_ler_id: le um ID inteiro positivo valido no intervalo [1, INT_MAX].
 * Pre: in != NULL, destino != NULL.
 * Pos: se retornar IO_OK, *destino >= 1.
 * Complexidade: O(k).
 */
IoStatus io_ler_id(FILE *in, int *destino);

/*
 * io_ler_opcao: le uma opcao inteira de menu no intervalo [min, max].
 * Pre: in != NULL, destino != NULL, min <= max.
 * Pos: se retornar IO_OK, *destino esta em [min, max].
 * Complexidade: O(k).
 */
IoStatus io_ler_opcao(FILE *in, int min, int max, int *destino);

/*
 * io_status_mensagem: retorna descricao legivel para o codigo IoStatus.
 * Complexidade: O(1).
 */
const char *io_status_mensagem(IoStatus status);

#endif /* IO_H */
