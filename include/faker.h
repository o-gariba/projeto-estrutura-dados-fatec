#ifndef FAKER_H
#define FAKER_H

/*
 * faker.h - Gerador pseudoaleatorio de registros de equipamentos de rede.
 *
 * Modulo em C11 puro (sem dependencias externas) usando PRNG xorshift32
 * deterministico quando inicializado com semente fixa, facilitando tanto
 * demonstracoes interativas quanto testes automatizados reproduziveis.
 */

#include "bst.h"
#include "equipamento.h"

#include <stddef.h>
#include <stdint.h>

#define FAKER_MAX_QTD 1000

typedef struct {
    uint32_t estado;
} Faker;

/*
 * faker_inicializar: inicializa o gerador pseudoaleatorio com uma semente.
 * Pre: fk != NULL (se semente == 0, adota constante padrao nao nula).
 * Pos: estado interno pronto para gerar sequencia pseudoaleatoria.
 * Complexidade: O(1).
 */
void faker_inicializar(Faker *fk, uint32_t semente);

/*
 * faker_proximo_u32: gera o proximo inteiro pseudoaleatorio de 32 bits.
 * Pre: fk != NULL.
 * Pos: atualiza o estado interno e retorna valor pseudoaleatorio.
 * Complexidade: O(1).
 */
uint32_t faker_proximo_u32(Faker *fk);

/*
 * faker_gerar_equipamento: aloca um Equipamento valido com dados ficticios do
 * dominio REDES (nome, tipo e localizacao) para o `id` informado.
 * Pre: fk != NULL, id > 0.
 * Pos: retorna ponteiro alocado na heap (posse do chamador) ou NULL em erro.
 * Complexidade: O(1).
 */
Equipamento *faker_gerar_equipamento(Faker *fk, int id);

/*
 * faker_popular_bst: gera e insere `qtd` equipamentos com IDs positivos unicos
 * (que nao colidem com os IDs ja existentes na arvore) na BST `arv`.
 * Pre: fk != NULL, arv != NULL, 1 <= qtd <= FAKER_MAX_QTD.
 * Pos: retorna a quantidade efetivamente inserida na BST.
 * Complexidade: O(qtd * h) em tempo (O(qtd log(n + qtd)) no caso medio).
 */
size_t faker_popular_bst(Faker *fk, BST *arv, size_t qtd);

#endif /* FAKER_H */
