#include "faker.h"

#include <limits.h>
#include <stdio.h>

#define FAKER_SEMENTE_PADRAO 0x9E3779B9u

static const char *const PREFIXOS_NOME[] = {
    "Core", "Dist", "Edge", "Access", "Gateway",
    "Firewall", "Switch", "Router", "Server", "AP"
};

static const char *const SITES_NOME[] = {
    "CPD", "Matriz", "DMZ", "Filial-SP", "Filial-RJ",
    "Campus", "Lab", "Norte", "Sul", "Cloud"
};

static const char *const TIPOS_EQUIP[] = {
    "Roteador", "Switch L2", "Switch L3", "Firewall", "Servidor",
    "Access Point", "Load Balancer", "Storage NAS", "Modem Fibra", "Gateway VPN"
};

static const char *const LOCAIS_EQUIP[] = {
    "Rack A1 - CPD", "Rack B2 - CPD", "Sala Redes 01", "Andar 2 - Ala A",
    "Andar 3 - Ala B", "Datacenter Sul", "Lab Informatica", "Portaria Central",
    "Auditorio", "Sala Servidores"
};

#define QTD_ITENS(arr) (sizeof(arr) / sizeof((arr)[0]))

void faker_inicializar(Faker *fk, uint32_t semente)
{
    if (fk == NULL) {
        return;
    }

    /* xorshift32 nao pode iniciar com estado zero (ficaria preso em 0). */
    fk->estado = (semente != 0u) ? semente : FAKER_SEMENTE_PADRAO;
}

uint32_t faker_proximo_u32(Faker *fk)
{
    uint32_t x;

    if (fk == NULL) {
        return 0u;
    }

    x = fk->estado;
    if (x == 0u) {
        x = FAKER_SEMENTE_PADRAO;
    }

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    fk->estado = x;
    return x;
}

Equipamento *faker_gerar_equipamento(Faker *fk, int id)
{
    char nome[TAM_TEXTO];
    const char *prefixo;
    const char *site;
    const char *tipo;
    const char *local;
    unsigned int numero;

    if (fk == NULL || id <= 0) {
        return NULL;
    }

    prefixo = PREFIXOS_NOME[faker_proximo_u32(fk) % QTD_ITENS(PREFIXOS_NOME)];
    site = SITES_NOME[faker_proximo_u32(fk) % QTD_ITENS(SITES_NOME)];
    numero = (unsigned int)(faker_proximo_u32(fk) % 99u) + 1u;
    tipo = TIPOS_EQUIP[faker_proximo_u32(fk) % QTD_ITENS(TIPOS_EQUIP)];
    local = LOCAIS_EQUIP[faker_proximo_u32(fk) % QTD_ITENS(LOCAIS_EQUIP)];

    snprintf(nome, sizeof(nome), "%s-%s-%02u", prefixo, site, numero);
    return equipamento_criar(id, nome, tipo, local);
}

static int sortear_id_unico(Faker *fk, const BST *arv, int faixa_max)
{
    int tentativa;
    int id;

    /* Tenta sorteio aleatorio direto primeiro (alta probabilidade de acerto). */
    for (tentativa = 0; tentativa < 64; tentativa++) {
        id = 1 + (int)(faker_proximo_u32(fk) % (uint32_t)faixa_max);
        if (bst_buscar(arv, id) == NULL) {
            return id;
        }
    }

    /* Fallback deterministico caso a faixa esteja densa. */
    id = 1 + (int)(faker_proximo_u32(fk) % (uint32_t)faixa_max);
    while (bst_buscar(arv, id) != NULL) {
        id = (id < INT_MAX) ? (id + 1) : 1;
    }
    return id;
}

size_t faker_popular_bst(Faker *fk, BST *arv, size_t qtd)
{
    size_t inseridos = 0;
    size_t i;
    size_t espaco;
    int faixa_max;

    if (fk == NULL || arv == NULL || qtd == 0 || qtd > FAKER_MAX_QTD) {
        return 0;
    }

    /* Faixa esparsa (~10x) mantem a arvore naturalmente balanceada na media. */
    espaco = (bst_tamanho(arv) + qtd) * 10;
    if (espaco < 1000) {
        espaco = 1000;
    }
    faixa_max = (espaco > (size_t)INT_MAX) ? INT_MAX : (int)espaco;

    for (i = 0; i < qtd; i++) {
        int id = sortear_id_unico(fk, arv, faixa_max);
        Equipamento *eq = faker_gerar_equipamento(fk, id);
        if (eq == NULL) {
            break;
        }
        if (bst_inserir(arv, eq) != BST_OK) {
            equipamento_destruir(eq);
            break;
        }
        inseridos++;
    }

    return inseridos;
}
