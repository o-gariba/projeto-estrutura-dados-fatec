# Guia de Estudo e Traços Visuais: Ponteiros, Memória, BST e Lista

Este guia complementa os diários `00` a `08` com **rastreamento passo a passo na memória (Stack vs. Heap)** para consolidar o estudo dos conceitos aplicados no projeto e apoiar apresentações ou provas.

---

## 1. Mapa de Memória (Stack vs. Heap) e Posse (*Ownership*)

Quando o programa está executando dentro de `app_executar()` ([src/app.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/src/app.c)), a memória se divide assim:

```text
STACK (escopo de app_executar)         HEAP (alocada com malloc)
┌───────────────────────────┐          ┌──────────────────────────────────┐
│ BST *arv ─────────────────┼─────────►│ BST { raiz ──► NoBST, tamanho }  │
└───────────────────────────┘          └─────────────────┬────────────────┘
                                                         ▼
                                       ┌──────────────────────────────────┐
                                       │ NoBST { dado ──► Equipamento,    │
                                       │         esq, dir }               │
                                       └─────────────────┬────────────────┘
                                                         ▼
                                       ┌──────────────────────────────────┐
                                       │ Equipamento { id, nome[80],      │
                                       │   tipo[80], localizacao[80] }    │
                                       └──────────────────────────────────┘

┌───────────────────────────┐          ┌──────────────────────────────────┐
│ Lista *lista ─────────────┼─────────►│ Lista { inicio, fim, tamanho }   │
└───────────────────────────┘          └─────────┬──────────────┬─────────┘
                                                 ▼              ▼
                                       [NoLista id|prox] ──► [NoLista id|NULL]
```

**Regras de posse (*ownership*) que evitam vazamentos e *double free*:**
1. Quem chama `equipamento_criar()` recebe um `Equipamento *` alocado na heap.
2. Se `bst_inserir(arv, eq)` retorna `BST_OK`, **a posse passa para a BST** — somente `bst_remover()` ou `bst_destruir()` darão `free` nele.
3. Se `bst_inserir()` falhar (ex.: `BST_ERRO_ID_DUPLICADO`), **a posse continua com o chamador** (`app.c` chama `equipamento_destruir(eq)`).
4. A `Lista` armazena apenas `int id` por valor dentro de cada `NoLista`; por isso ela não compartilha ponteiros com a BST e permanece intacta mesmo quando um ID é removido da árvore.

---

## 2. Traço Passo a Passo na BST (Inserção e Remoção com 2 Filhos)

Considere a sequência de inserções do caso E2E `01-bst-operacoes.in`: `50, 30, 70, 20, 40, 60, 80, 65`.

```text
Estado após as 8 inserções:
            50
          /    \
        30      70
       /  \    /  \
     20   40  60   80
                \
                 65
```

### Passo A: Remoção de folha (`id = 20`) — Caso 1
- `remover_recursivo(50, 20)` desce para `30`, depois para `20`.
- `20->esq == NULL && 20->dir == NULL`: libera `20->dado`, dá `free(20)` e retorna `NULL`.
- `30->esq` recebe `NULL`.

### Passo B: Remoção com 1 filho (`id = 60`) — Caso 2a
- `60` tem `esq == NULL` e `dir == 65`.
- Guarda `filho = 65`, libera `60->dado` e `60`, e retorna `65`.
- `70->esq` passa a apontar diretamente para `65` ("adoção do neto").

### Passo C: Remoção da raiz com 2 filhos (`id = 50`) — Caso 3
Estado antes de remover `50`:
```text
            50
          /    \
        30      70
          \    /  \
          40  65   80
```
1. Como `50->esq != NULL` e `50->dir != NULL`, buscamos o **sucessor em-ordem** (menor nó da subárvore direita):
   - `pai_suc = 50`, `suc = 70`.
   - Como `70->esq` é `65`, o laço `while (suc->esq != NULL)` avança uma vez: `pai_suc = 70`, `suc = 65`.
   - Agora `65->esq == NULL`: encontramos o sucessor (`65`).
2. Destruímos `50->dado` (`equipamento_destruir`) e transferimos o ponteiro: `50->dado = suc->dado` (o equipamento `65`).
3. Desconectamos `suc`: como `pai_suc (70) != no (50)`, executamos `pai_suc->esq = suc->dir` (`70->esq = NULL`, ou o filho direito de `suc` caso existisse!).
4. Executamos `free(suc)` **apenas no `NoBST`**, sem destruir `suc->dado`.

Estado final e percurso em-ordem (`esq -> raiz -> dir`):
```text
            65
          /    \
        30      70
          \       \
          40       80
```
Saída em-ordem: `30, 40, 65, 70, 80` — a invariante da BST permanece intacta.

---

## 3. Traço Passo a Passo na Lista Encadeada Simples

1. **Lista vazia inicial:** `inicio = NULL`, `fim = NULL`, `tamanho = 0`.
2. **`lista_inserir_fim(lista, 500)`:**
   `inicio` e `fim` apontam para `[500 | NULL]`, `tamanho = 1`.
3. **`lista_inserir_fim(lista, 777)`:**
   `fim->prox = [777 | NULL]`; `fim = [777 | NULL]`, `tamanho = 2` (custo $O(1)$, sem percorrer a lista).
4. **`lista_remover_inicio(lista, &id)` duas vezes:**
   - 1ª remoção: `id = 500`; `inicio` avança para `[777 | NULL]`; `fim` continua em `[777 | NULL]`, `tamanho = 1`.
   - 2ª remoção: `id = 777`; `inicio` recebe `alvo->prox` (`NULL`). O `if (lista->inicio == NULL)` zera **`lista->fim = NULL`** para não deixar `fim` apontando para o bloco liberado (*dangling pointer*).
5. **3ª remoção em lista vazia:** `lista->inicio == NULL` retorna `LISTA_ERRO_VAZIA` imediatamente, sem tocar em `*id_removido` nem falhar.

---

## 4. Fluxo Seguro de Entrada (`io.c` + `app.c`)

1. `io_ler_linha(in, buf)` lê para `char tmp[81]` (`79` caracteres + `\n` + `\0`).
2. Se não houver `\n` e `strlen(tmp) == 80`, significa que o usuário digitou 80+ caracteres úteis: `descartar_resto_linha(in)` consome o resto até `\n` ou `EOF` e retorna `IO_ERRO_LONGO`, impedindo que a próxima leitura consuma lixo do buffer.
3. Em `io_ler_inteiro_intervalo`, `strtol` valida dígitos e sufixos (`endptr`), e a checagem dupla `errno == ERANGE || valor < INT_MIN || valor > INT_MAX` impede que números como `99999999999` (válidos em `long` de 64 bits) sofram truncamento silencioso ao converter para `int` de 32 bits.
4. Ao receber `EOF` em qualquer prompt de `app.c`, `solicitar_id` / `solicitar_texto` retornam `0`, encerrando o laço principal e executando `bst_destruir` e `lista_destruir`.
