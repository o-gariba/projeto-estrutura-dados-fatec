#if defined(__unix__) || defined(__APPLE__)
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#endif

#include "app.h"

#include "bst.h"
#include "equipamento.h"
#include "faker.h"
#include "io.h"
#include "lista.h"

#include <limits.h>
#include <stdlib.h>
#include <time.h>

static int detectar_modo_interativo(FILE *in, const AppConfig *cfg)
{
    if (cfg != NULL && cfg->modo_interativo >= 0) {
        return cfg->modo_interativo != 0;
    }

#if defined(__unix__) || defined(__APPLE__)
    {
        int fd = fileno(in);
        if (fd >= 0) {
            return isatty(fd) != 0;
        }
    }
#else
    (void)in;
#endif

    return 0;
}

static void limpar_tela(FILE *out)
{
    /* Sequencia ANSI: limpa a tela e move o cursor para o canto superior esquerdo. */
    fputs("\033[2J\033[H", out);
    fflush(out);
}

/*
 * Aguarda o usuario pressionar ENTER para que leia o retorno antes de limpar a tela.
 * Retorna 1 ao concluir a pausa, ou 0 se encontrar EOF.
 */
static int pausar_tela(FILE *in, FILE *out)
{
    int ch;

    fprintf(out, "\nPressione ENTER para continuar...");
    fflush(out);

    while ((ch = fgetc(in)) != '\n') {
        if (ch == EOF) {
            return 0;
        }
    }

    return 1;
}

static void iniciar_bloco_saida(FILE *out)
{
    fputs("\n------------------------------------------\n", out);
}

static void encerrar_bloco_saida(FILE *out)
{
    fputs("------------------------------------------\n", out);
    fflush(out);
}

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
            "8 - Exibir BST em formato de arvore\n"
            "9 - Gerar registros aleatorios na BST (Faker)\n"
            "5 - Inserir ID na estrutura linear\n"
            "6 - Remover da estrutura linear\n"
            "7 - Exibir estrutura linear\n"
            "0 - Encerrar\n"
            "==========================================\n"
            "Escolha uma opcao: ");
    fflush(out);
}

/*
 * Retorna 1 se leu um inteiro valido no intervalo [min, max], ou 0 se encontrou EOF.
 */
static int solicitar_inteiro(FILE *in,
                             FILE *out,
                             const char *prompt,
                             int min,
                             int max,
                             const char *dica_erro,
                             int *destino)
{
    for (;;) {
        IoStatus st;
        fprintf(out, "%s", prompt);
        fflush(out);

        st = io_ler_inteiro_intervalo(in, min, max, destino);
        if (st == IO_OK) {
            return 1;
        }
        if (st == IO_EOF) {
            return 0;
        }
        fprintf(out, "Erro: %s %s\n", io_status_mensagem(st), dica_erro);
    }
}

static int solicitar_id(FILE *in, FILE *out, const char *prompt, int *id)
{
    return solicitar_inteiro(in, out, prompt, 1, INT_MAX,
                             "Informe um ID inteiro positivo.", id);
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
        iniciar_bloco_saida(out);
        fprintf(out, "Erro: ID %d ja cadastrado na BST.\n", id);
        encerrar_bloco_saida(out);
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
        iniciar_bloco_saida(out);
        fprintf(out, "Erro: nao foi possivel criar o equipamento.\n");
        encerrar_bloco_saida(out);
        return 1;
    }

    st = bst_inserir(arv, eq);
    iniciar_bloco_saida(out);
    if (st != BST_OK) {
        equipamento_destruir(eq);
        fprintf(out, "Erro: falha ao inserir equipamento na BST.\n");
    } else {
        fprintf(out, "Equipamento ID %d cadastrado na BST.\n", id);
    }
    encerrar_bloco_saida(out);
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
    iniciar_bloco_saida(out);
    if (eq == NULL) {
        fprintf(out, "Equipamento ID %d nao encontrado na BST.\n", id);
    } else {
        equipamento_imprimir(eq, out);
    }
    encerrar_bloco_saida(out);
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
    iniciar_bloco_saida(out);
    if (st == BST_OK) {
        fprintf(out, "Equipamento ID %d removido da BST.\n", id);
    } else {
        fprintf(out, "Equipamento ID %d nao encontrado na BST.\n", id);
    }
    encerrar_bloco_saida(out);
    return 1;
}

static void tratar_listar_bst(FILE *out, const BST *arv)
{
    iniciar_bloco_saida(out);
    if (bst_vazia(arv)) {
        fprintf(out, "BST vazia. Nenhum equipamento cadastrado.\n");
    } else {
        fprintf(out, "--- Equipamentos em-ordem (total: %zu) ---\n",
                bst_tamanho(arv));
        bst_listar_em_ordem(arv, out);
    }
    encerrar_bloco_saida(out);
}

static void tratar_imprimir_arvore_bst(FILE *out, const BST *arv)
{
    iniciar_bloco_saida(out);
    if (bst_vazia(arv)) {
        fprintf(out, "BST vazia. Nenhum equipamento cadastrado.\n");
    } else {
        fprintf(out,
                "--- Arvore BST (total: %zu | [E] esquerda, [D] direita) ---\n",
                bst_tamanho(arv));
        bst_imprimir_arvore(arv, out);
    }
    encerrar_bloco_saida(out);
}

static int tratar_gerar_faker_bst(FILE *in, FILE *out, BST *arv, Faker *fk)
{
    int qtd = 0;
    size_t inseridos;
    char prompt[80];
    char dica[80];

    snprintf(prompt, sizeof(prompt),
             "Quantidade de registros a gerar (1 a %d): ", FAKER_MAX_QTD);
    snprintf(dica, sizeof(dica),
             "Informe um numero inteiro entre 1 e %d.", FAKER_MAX_QTD);

    if (!solicitar_inteiro(in, out, prompt, 1, FAKER_MAX_QTD, dica, &qtd)) {
        return 0;
    }

    inseridos = faker_popular_bst(fk, arv, (size_t)qtd);
    iniciar_bloco_saida(out);
    if (inseridos == (size_t)qtd) {
        fprintf(out,
                "Foram gerados e inseridos %zu registros na BST (total: %zu).\n",
                inseridos, bst_tamanho(arv));
    } else {
        fprintf(out,
                "Aviso: gerados %zu de %d registros solicitados (total: %zu).\n",
                inseridos, qtd, bst_tamanho(arv));
    }
    encerrar_bloco_saida(out);
    return 1;
}

static int tratar_inserir_linear(FILE *in, FILE *out, Lista *lista)
{
    int id;
    ListaStatus st;

    if (!solicitar_id(in, out, "ID para inserir na lista encadeada: ", &id)) {
        return 0;
    }

    st = lista_inserir_fim(lista, id);
    iniciar_bloco_saida(out);
    if (st == LISTA_OK) {
        fprintf(out, "ID %d inserido no fim da lista encadeada.\n", id);
    } else {
        fprintf(out, "Erro: falha ao inserir ID na lista encadeada.\n");
    }
    encerrar_bloco_saida(out);
    return 1;
}

static void tratar_remover_linear(FILE *out, Lista *lista)
{
    int id = 0;
    ListaStatus st = lista_remover_inicio(lista, &id);

    iniciar_bloco_saida(out);
    if (st == LISTA_ERRO_VAZIA) {
        fprintf(out, "Aviso: estrutura linear vazia. Nada a remover.\n");
    } else if (st == LISTA_OK) {
        fprintf(out, "ID %d removido do inicio da lista encadeada.\n", id);
    } else {
        fprintf(out, "Erro: falha ao remover da estrutura linear.\n");
    }
    encerrar_bloco_saida(out);
}

static void tratar_exibir_linear(FILE *out, const Lista *lista)
{
    iniciar_bloco_saida(out);
    if (lista_vazia(lista)) {
        fprintf(out, "Estrutura linear vazia.\n");
    } else {
        fprintf(out, "Lista (inicio -> fim) [%zu]: ", lista_tamanho(lista));
        lista_exibir(lista, out);
    }
    encerrar_bloco_saida(out);
}

int app_executar_config(FILE *in, FILE *out, const AppConfig *cfg)
{
    BST *arv;
    Lista *lista;
    Faker faker;
    uint32_t semente;
    int interativo;
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

    semente = (cfg != NULL && cfg->usar_semente_fixa)
                  ? cfg->semente
                  : (uint32_t)time(NULL);
    faker_inicializar(&faker, semente);
    interativo = detectar_modo_interativo(in, cfg);

    if (interativo) {
        limpar_tela(out);
    }

    while (executando) {
        int opcao = -1;
        IoStatus st;
        int pausar = 1;

        exibir_menu(out);
        st = io_ler_opcao(in, 0, 9, &opcao);

        if (st == IO_EOF) {
            fprintf(out, "\nFim da entrada. Encerrando sistema...\n");
            break;
        }

        if (st != IO_OK) {
            iniciar_bloco_saida(out);
            fprintf(out, "Erro: %s Escolha uma opcao entre 0 e 9.\n",
                    io_status_mensagem(st));
            encerrar_bloco_saida(out);
        } else {
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
                case 8:
                    tratar_imprimir_arvore_bst(out, arv);
                    break;
                case 9:
                    executando = tratar_gerar_faker_bst(in, out, arv, &faker);
                    break;
                case 0:
                    iniciar_bloco_saida(out);
                    fprintf(out, "Encerrando sistema e liberando memoria...\n");
                    encerrar_bloco_saida(out);
                    executando = 0;
                    pausar = 0;
                    break;
                default:
                    break;
            }
        }

        if (!executando && pausar) {
            fprintf(out, "\nFim da entrada. Encerrando sistema...\n");
        } else if (interativo && pausar) {
            if (!pausar_tela(in, out)) {
                fprintf(out, "\nFim da entrada. Encerrando sistema...\n");
                break;
            }
            limpar_tela(out);
        }
    }

    bst_destruir(arv);
    lista_destruir(lista);
    return EXIT_SUCCESS;
}

int app_executar(FILE *in, FILE *out)
{
    AppConfig cfg;

    cfg.semente = 0u;
    cfg.usar_semente_fixa = 0;
    cfg.modo_interativo = -1;
    return app_executar_config(in, out, &cfg);
}
