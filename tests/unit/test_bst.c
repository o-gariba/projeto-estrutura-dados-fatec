/*
 * test_bst.c - Testes unitarios da Arvore Binaria de Busca (bst).
 */
#include "bst.h"
#include "test.h"

#include <stdio.h>
#include <string.h>

static Equipamento *novo_eq(int id, const char *nome)
{
    return equipamento_criar(id, nome, "Roteador", "CPD");
}

TEST(deve_criar_arvore_vazia_e_tratar_operacoes)
{
    BST *arv = bst_criar();
    ASSERT_NOT_NULL(arv);
    ASSERT_EQ_INT(1, bst_vazia(arv));
    ASSERT_EQ_INT(0, (int)bst_tamanho(arv));
    ASSERT_NULL(bst_buscar(arv, 10));
    ASSERT_EQ_INT(BST_ERRO_NAO_ENCONTRADO, bst_remover(arv, 10));

    bst_destruir(arv);
    bst_destruir(NULL);
}

TEST(deve_inserir_buscar_e_rejeitar_id_duplicado)
{
    BST *arv = bst_criar();
    Equipamento *dup;
    const Equipamento *achado;

    ASSERT_NOT_NULL(arv);
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(50, "Core-50")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(30, "Dist-30")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(70, "Edge-70")));
    ASSERT_EQ_INT(3, (int)bst_tamanho(arv));
    ASSERT_EQ_INT(0, bst_vazia(arv));

    /* Rejeicao de ID duplicado mantendo o registro original intacto */
    dup = novo_eq(30, "Duplicado");
    ASSERT_NOT_NULL(dup);
    ASSERT_EQ_INT(BST_ERRO_ID_DUPLICADO, bst_inserir(arv, dup));
    equipamento_destruir(dup);
    ASSERT_EQ_INT(3, (int)bst_tamanho(arv));

    achado = bst_buscar(arv, 30);
    ASSERT_NOT_NULL(achado);
    ASSERT_EQ_STR("Dist-30", achado->nome);

    ASSERT_NULL(bst_buscar(arv, 99));
    ASSERT_NULL(bst_buscar(arv, 0));
    ASSERT_NULL(bst_buscar(arv, -1));

    bst_destruir(arv);
}

TEST(deve_listar_em_ordem_crescente_de_id)
{
    BST *arv = bst_criar();
    FILE *out = tmpfile();
    char linha[256];

    ASSERT_NOT_NULL(arv);
    ASSERT_NOT_NULL(out);

    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(40, "Eq-40")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(20, "Eq-20")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(60, "Eq-60")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(10, "Eq-10")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(30, "Eq-30")));

    bst_listar_em_ordem(arv, out);
    rewind(out);

    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT(strstr(linha, "ID: 10 |") != NULL);
    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT(strstr(linha, "ID: 20 |") != NULL);
    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT(strstr(linha, "ID: 30 |") != NULL);
    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT(strstr(linha, "ID: 40 |") != NULL);
    ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
    ASSERT(strstr(linha, "ID: 60 |") != NULL);
    ASSERT_NULL(fgets(linha, (int)sizeof(linha), out));

    fclose(out);
    bst_destruir(arv);
}

TEST(deve_remover_no_folha_e_no_com_um_filho)
{
    BST *arv = bst_criar();
    ASSERT_NOT_NULL(arv);

    /*
     *         50
     *        /  \
     *      30    70
     *     /        \
     *   20          80
     */
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(50, "Eq-50")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(30, "Eq-30")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(70, "Eq-70")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(20, "Eq-20")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(80, "Eq-80")));

    /* Caso 1: folha (20) */
    ASSERT_EQ_INT(BST_OK, bst_remover(arv, 20));
    ASSERT_NULL(bst_buscar(arv, 20));
    ASSERT_EQ_INT(4, (int)bst_tamanho(arv));

    /* Agora 30 ficou folha; remover 70 que tem 1 filho a direita (80) */
    ASSERT_EQ_INT(BST_OK, bst_remover(arv, 70));
    ASSERT_NULL(bst_buscar(arv, 70));
    ASSERT_NOT_NULL(bst_buscar(arv, 80));
    ASSERT_EQ_INT(3, (int)bst_tamanho(arv));

    /* Adicionar 10 sob 30 para testar 1 filho a esquerda */
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(10, "Eq-10")));
    ASSERT_EQ_INT(BST_OK, bst_remover(arv, 30));
    ASSERT_NULL(bst_buscar(arv, 30));
    ASSERT_NOT_NULL(bst_buscar(arv, 10));
    ASSERT_EQ_INT(3, (int)bst_tamanho(arv));

    bst_destruir(arv);
}

TEST(deve_remover_no_com_dois_filhos_e_raiz)
{
    BST *arv = bst_criar();
    FILE *out = tmpfile();
    char linha[256];
    ASSERT_NOT_NULL(arv);
    ASSERT_NOT_NULL(out);

    /*
     *         50
     *        /  \
     *      30    70
     *     /  \  /  \
     *   20  40 60  80
     *            \
     *            65
     */
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(50, "Eq-50")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(30, "Eq-30")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(70, "Eq-70")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(20, "Eq-20")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(40, "Eq-40")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(60, "Eq-60")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(80, "Eq-80")));
    ASSERT_EQ_INT(BST_OK, bst_inserir(arv, novo_eq(65, "Eq-65")));

    /* Remover raiz (50) com 2 filhos: sucessor eh 60, que tem filho direito 65 */
    ASSERT_EQ_INT(BST_OK, bst_remover(arv, 50));
    ASSERT_NULL(bst_buscar(arv, 50));
    ASSERT_NOT_NULL(bst_buscar(arv, 60));
    ASSERT_NOT_NULL(bst_buscar(arv, 65));
    ASSERT_EQ_INT(7, (int)bst_tamanho(arv));

    /* Remover subarvore (30) com 2 filhos: sucessor imediato eh 40 */
    ASSERT_EQ_INT(BST_OK, bst_remover(arv, 30));
    ASSERT_NULL(bst_buscar(arv, 30));
    ASSERT_NOT_NULL(bst_buscar(arv, 20));
    ASSERT_NOT_NULL(bst_buscar(arv, 40));
    ASSERT_EQ_INT(6, (int)bst_tamanho(arv));

    /* Verificar ordem final em-ordem: 20, 40, 60, 65, 70, 80 */
    bst_listar_em_ordem(arv, out);
    rewind(out);
    {
        int esperados[] = {20, 40, 60, 65, 70, 80};
        int i;
        for (i = 0; i < 6; i++) {
            char padrao[32];
            snprintf(padrao, sizeof(padrao), "ID: %d |", esperados[i]);
            ASSERT_NOT_NULL(fgets(linha, (int)sizeof(linha), out));
            ASSERT(strstr(linha, padrao) != NULL);
        }
        ASSERT_NULL(fgets(linha, (int)sizeof(linha), out));
    }

    fclose(out);
    bst_destruir(arv);
}

int main(void)
{
    printf("\n=== Executando Testes da BST ===\n");

    RUN_TEST(deve_criar_arvore_vazia_e_tratar_operacoes);
    RUN_TEST(deve_inserir_buscar_e_rejeitar_id_duplicado);
    RUN_TEST(deve_listar_em_ordem_crescente_de_id);
    RUN_TEST(deve_remover_no_folha_e_no_com_um_filho);
    RUN_TEST(deve_remover_no_com_dois_filhos_e_raiz);

    return TEST_RESUMO();
}
