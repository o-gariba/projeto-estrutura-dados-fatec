#include "equipamento.h"

#include <stdlib.h>
#include <string.h>

static void copiar_campo(char destino[TAM_TEXTO], const char *origem)
{
    size_t len = strlen(origem);
    /*
     * Como io_texto_valido() ja garantiu len <= TAM_TEXTO - 1,
     * memcpy com len + 1 copia toda a string incluindo '\0' sem truncamento.
     */
    memcpy(destino, origem, len + 1);
}

Equipamento *equipamento_criar(int id,
                               const char *nome,
                               const char *tipo,
                               const char *localizacao)
{
    Equipamento *eq;

    if (id <= 0) {
        return NULL;
    }

    if (!io_texto_valido(nome) || !io_texto_valido(tipo) ||
        !io_texto_valido(localizacao)) {
        return NULL;
    }

    eq = malloc(sizeof(*eq));
    if (eq == NULL) {
        return NULL;
    }

    eq->id = id;
    copiar_campo(eq->nome, nome);
    copiar_campo(eq->tipo, tipo);
    copiar_campo(eq->localizacao, localizacao);

    return eq;
}

void equipamento_destruir(Equipamento *eq)
{
    free(eq);
}

void equipamento_imprimir(const Equipamento *eq, FILE *out)
{
    if (eq == NULL || out == NULL) {
        return;
    }

    fprintf(out, "ID: %d | Nome: %s | Tipo: %s | Localizacao: %s\n",
            eq->id, eq->nome, eq->tipo, eq->localizacao);
}
