#include "app.h"

#include "bst.h"
#include "equipamento.h"
#include "io.h"
#include "lista.h"

#include <stdlib.h>

static void exibir_menu(FILE *out)
{
    fprintf(out,
            "\n"
            "==========================================\n"
            "SISTEMA DE ESTRUTURA DE DADOS\n"
            "==========================================\n"
            "1 - Cadastrar registro na BST\n"
            "2 - Buscar registro por ID na BST\n"
            "3 - Remover registro da BST\n"
            "4 - Listar registros em-ordem\n"
            "5 - Inserir ID na estrutura linear\n"
            "6 - Remover da estrutura linear\n"
            "7 - Exibir estrutura linear\n"
            "0 - Encerrar\n"
            "==========================================\n"
            "Escolha uma opcao: ");
    fflush(out);
}

/*
 * Retorna 1 se leu um ID valido, ou 0 se encontrou EOF.
 */
static int solicitar_id(FILE *in, FILE *out, const char *prompt, int *id)
{
    for (;;) {
        IoStatus st;
        fprintf(out, "%s", prompt);
        fflush(out);

        st = io_ler_id(in, id);
        if (st == IO_OK) {
            return 1;
        }
        if (st == IO_EOF) {
            return 0;
        }
        fprintf(out, "Erro: %s Informe um ID inteiro positivo.\n",
                io_status_mensagem(st));
    }
}

/*
 * Retorna 1 se leu um texto valido, ou 0 se encontrou EOF.
 */
static int solicitar_texto(FILE *in,
                           FILE *out,
                           const char *prompt,
                           char destino[TAM_TEXTO])
{
    for (;;) {
        IoStatus st;
        fprintf(out, "%s", prompt);
        fflush(out);

        st = io_ler_texto(in, destino);
        if (st == IO_OK) {
            return 1;
        }
        if (st == IO_EOF) {
            return 0;
        }
        fprintf(out, "Erro: %s\n", io_status_mensagem(st));
    }
}

static int tratar_cadastrar_bst(FILE *in, FILE *out, BST *arv)
{
    int id;
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];
    char localizacao[TAM_TEXTO];
    Equipamento *eq;
    BstStatus st;

    if (!solicitar_id(in, out, "ID do equipamento: ", &id)) {
        return 0;
    }

    if (bst_buscar(arv, id) != NULL) {
        fprintf(out, "Erro: ID %d ja cadastrado na BST.\n", id);
        return 1;
    }

    if (!solicitar_texto(in, out, "Nome do equipamento: ", nome)) {
        return 0;
    }
    if (!solicitar_texto(in, out, "Tipo do equipamento: ", tipo)) {
        return 0;
    }
    if (!solicitar_texto(in, out, "Localizacao: ", localizacao)) {
        return 0;
    }

    eq = equipamento_criar(id, nome, tipo, localizacao);
    if (eq == NULL) {
        fprintf(out, "Erro: nao foi possivel criar o equipamento.\n");
        return 1;
    }

    st = bst_inserir(arv, eq);
    if (st != BST_OK) {
        equipamento_destruir(eq);
        fprintf(out, "Erro: falha ao inserir equipamento na BST.\n");
        return 1;
    }

    fprintf(out, "Equipamento ID %d cadastrado na BST.\n", id);
    return 1;
}

static int tratar_buscar_bst(FILE *in, FILE *out, const BST *arv)
{
    int id;
    const Equipamento *eq;

    if (!solicitar_id(in, out, "ID para buscar na BST: ", &id)) {
        return 0;
    }

    eq = bst_buscar(arv, id);
    if (eq == NULL) {
        fprintf(out, "Equipamento ID %d nao encontrado na BST.\n", id);
        return 1;
    }

    equipamento_imprimir(eq, out);
    return 1;
}

static int tratar_remover_bst(FILE *in, FILE *out, BST *arv)
{
    int id;
    BstStatus st;

    if (!solicitar_id(in, out, "ID para remover da BST: ", &id)) {
        return 0;
    }

    st = bst_remover(arv, id);
    if (st == BST_OK) {
        fprintf(out, "Equipamento ID %d removido da BST.\n", id);
    } else {
        fprintf(out, "Equipamento ID %d nao encontrado na BST.\n", id);
    }
    return 1;
}

static void tratar_listar_bst(FILE *out, const BST *arv)
{
    if (bst_vazia(arv)) {
        fprintf(out, "BST vazia. Nenhum equipamento cadastrado.\n");
        return;
    }

    fprintf(out, "--- Equipamentos em-ordem (total: %zu) ---\n",
            bst_tamanho(arv));
    bst_listar_em_ordem(arv, out);
}

static int tratar_inserir_linear(FILE *in, FILE *out, Lista *lista)
{
    int id;
    ListaStatus st;

    if (!solicitar_id(in, out, "ID para inserir na lista encadeada: ", &id)) {
        return 0;
    }

    st = lista_inserir_fim(lista, id);
    if (st == LISTA_OK) {
        fprintf(out, "ID %d inserido no fim da lista encadeada.\n", id);
    } else {
        fprintf(out, "Erro: falha ao inserir ID na lista encadeada.\n");
    }
    return 1;
}

static void tratar_remover_linear(FILE *out, Lista *lista)
{
    int id = 0;
    ListaStatus st = lista_remover_inicio(lista, &id);

    if (st == LISTA_ERRO_VAZIA) {
        fprintf(out, "Aviso: estrutura linear vazia. Nada a remover.\n");
    } else if (st == LISTA_OK) {
        fprintf(out, "ID %d removido do inicio da lista encadeada.\n", id);
    } else {
        fprintf(out, "Erro: falha ao remover da estrutura linear.\n");
    }
}

static void tratar_exibir_linear(FILE *out, const Lista *lista)
{
    if (lista_vazia(lista)) {
        fprintf(out, "Estrutura linear vazia.\n");
        return;
    }

    fprintf(out, "Lista (inicio -> fim) [%zu]: ", lista_tamanho(lista));
    lista_exibir(lista, out);
}

int app_executar(FILE *in, FILE *out)
{
    BST *arv;
    Lista *lista;
    int executando = 1;

    if (in == NULL || out == NULL) {
        return EXIT_FAILURE;
    }

    arv = bst_criar();
    lista = lista_criar();
    if (arv == NULL || lista == NULL) {
        bst_destruir(arv);
        lista_destruir(lista);
        fprintf(out, "Erro fatal: falha ao alocar estruturas principais.\n");
        return EXIT_FAILURE;
    }

    while (executando) {
        int opcao = -1;
        IoStatus st;

        exibir_menu(out);
        st = io_ler_opcao(in, 0, 7, &opcao);

        if (st == IO_EOF) {
            fprintf(out, "\nFim da entrada. Encerrando sistema...\n");
            break;
        }
        if (st != IO_OK) {
            fprintf(out, "Erro: %s Escolha uma opcao entre 0 e 7.\n",
                    io_status_mensagem(st));
            continue;
        }

        switch (opcao) {
            case 1:
                executando = tratar_cadastrar_bst(in, out, arv);
                break;
            case 2:
                executando = tratar_buscar_bst(in, out, arv);
                break;
            case 3:
                executando = tratar_remover_bst(in, out, arv);
                break;
            case 4:
                tratar_listar_bst(out, arv);
                break;
            case 5:
                executando = tratar_inserir_linear(in, out, lista);
                break;
            case 6:
                tratar_remover_linear(out, lista);
                break;
            case 7:
                tratar_exibir_linear(out, lista);
                break;
            case 0:
                fprintf(out, "Encerrando sistema e liberando memoria...\n");
                executando = 0;
                break;
            default:
                break;
        }
    }

    bst_destruir(arv);
    lista_destruir(lista);
    return EXIT_SUCCESS;
}
