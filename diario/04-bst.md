# Etapa 04: Árvore Binária de Busca (`bst.h` e `bst.c`)

**Requisitos atendidos:** R02 (inserção, busca, remoção com 0, 1 e 2 filhos, listagem em-ordem), R03 (rejeição de ID duplicado/inválido), R07 (liberação de memória), R09 (separação obrigatória em `bst.h` e `bst.c`)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Implementar o núcleo de indexação dos equipamentos em uma **Árvore Binária de Busca (BST — *Binary Search Tree*)** separada em [include/bst.h](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/include/bst.h) e [src/bst.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/src/bst.c), garantindo todas as operações obrigatórias sem vazamentos ou corrupção de memória.

---

## 2. Ideia Central e Modelo Mental

Uma **BST** organiza os nós usando a **invariante de ordenação por chave** (aqui, `eq->id`):
- Todo nó na **subárvore esquerda** possui `id < no->dado->id`;
- Todo nó na **subárvore direita** possui `id > no->dado->id`;
- Não existem IDs repetidos (igualdade é rejeitada na inserção).

Essa propriedade garante duas consequências imediatas:
1. **Busca binária estrutural:** a cada comparação, descartamos metade dos caminhos possíveis (esquerda ou direita).
2. **Percurso em-ordem (`esq -> raiz -> dir`):** visitar a subárvore esquerda, depois a raiz e depois a direita produz automaticamente os equipamentos **ordenados em ordem crescente de ID**, sem precisar de algoritmo de ordenação externo.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Posse de Memória (*Ownership*) bem definida

Em C, quando um TAD guarda ponteiros alocados externamente (`Equipamento *`), é fundamental definir quem dá `free()`:
- **Se `bst_inserir(arv, eq)` retorna `BST_OK`:** a BST assume a posse do `Equipamento *eq` e se responsabiliza por liberá-lo em `bst_remover()` ou `bst_destruir()`.
- **Se `bst_inserir(arv, eq)` retorna erro** (ex.: `BST_ERRO_ID_DUPLICADO`): a árvore **não** assume posse nem libera `eq`; quem chamou decide o que fazer (e evita-se alocar à toa consultando `bst_buscar()` antes).

### 3.2. Inserção recursiva sem alocar antes da hora

Na função `inserir_recursivo()`, descemos na árvore comparando `eq->id` com `no->dado->id`. Só chamamos `malloc(sizeof(NoBST))` quando encontramos o ponteiro `NULL` onde o novo nó deve nascer. Se encontrarmos um ID igual no caminho, retornamos `BST_ERRO_ID_DUPLICADO` imediatamente sem tocar na heap.

### 3.3. Os 3 Casos de Remoção em uma BST

A remoção (`remover_recursivo`) é a operação mais delicada da BST porque precisa **preservar a invariante** para todos os nós restantes:

1. **Caso 1 — 0 filhos (nó folha):**
   Libera `no->dado` (`equipamento_destruir`), libera `no` e retorna `NULL` para o pai (que desconecta o ponteiro).
2. **Caso 2 — 1 filho (só esquerda ou só direita):**
   Guarda o ponteiro do único filho (`filho = no->dir` ou `filho = no->esq`), libera `no->dado` e `no`, e retorna `filho` para o pai "adotar" o neto diretamente.
3. **Caso 3 — 2 filhos:**
   Não podemos simplesmente apagar o nó porque há duas subárvores para reconectar. A solução clássica substitui o conteúdo do nó pelo seu **sucessor em-ordem** (o menor ID da subárvore direita — obtido descendo sempre à esquerda a partir de `no->dir`), ou pelo predecessor em-ordem.
   - Destruímos o `Equipamento` antigo do nó alvo (`equipamento_destruir(no->dado)`).
   - Transferimos o ponteiro `no->dado = suc->dado`.
   - Desconectamos `suc` do seu pai (`pai_suc`):
     - Se `pai_suc == no` (o sucessor era o filho direito imediato de `no`), fazemos `pai_suc->dir = suc->dir`.
     - Caso contrário, fazemos `pai_suc->esq = suc->dir` (o sucessor nunca tem filho esquerdo, mas pode ter filho direito!).
   - Chamamos **`free(suc)`** apenas no nó estrutural — **sem** chamar `equipamento_destruir(suc->dado)`, pois esse equipamento agora pertence a `no`!

### 3.4. Por que a destruição usa percurso Pós-Ordem?

Em `destruir_recursivo(NoBST *no)`:
```c
destruir_recursivo(no->esq);
destruir_recursivo(no->dir);
equipamento_destruir(no->dado);
free(no);
```
Se liberássemos `no` antes de visitar `no->esq` e `no->dir` (pré-ordem), ler `no->esq` e `no->dir` após `free(no)` seria **uso após liberação (*Use-After-Free*)** — comportamento indefinido! O percurso **pós-ordem** garante que os filhos já foram completamente liberados antes de o pai ser destruído.

---

## 4. Complexidade das Operações

Seja $n$ o número de equipamentos na BST e $h$ a altura da árvore:

| Operação | Tempo (caso médio / balanceada) | Tempo (pior caso / degenerada) | Espaço auxiliar |
|---|---|---|---|
| `bst_criar` | $O(1)$ | $O(1)$ | $O(1)$ |
| `bst_inserir` | $O(\log n)$ | $O(n)$ | $O(h)$ (pilha de recursão) |
| `bst_buscar` | $O(\log n)$ | $O(n)$ | $O(1)$ (versão iterativa) |
| `bst_remover` | $O(\log n)$ | $O(n)$ | $O(h)$ (pilha de recursão) |
| `bst_listar_em_ordem` | $O(n)$ | $O(n)$ | $O(h)$ |
| `bst_destruir` | $O(n)$ | $O(n)$ | $O(h)$ |

> **Observação:** Inserir IDs em ordem estritamente crescente (`1, 2, 3, 4...`) degenera a BST em uma lista encadeada de altura $h = n$, levando busca/inserção/remoção para $O(n)$. Árvores autobalanceadas (AVL, Rubro-Negra) resolvem isso mantendo $h = O(\log n)$, mas fogem ao escopo deste projeto.

---

## 5. Armadilhas Comuns e Como Evitamos

1. ***Double free* ao remover nó com 2 filhos:** quem chama `remover_recursivo` recursivamente para apagar o nó sucessor muitas vezes esquece que o `Equipamento *` foi movido e acaba dando `free` nele duas vezes. Desconectar `suc` diretamente via `pai_suc` elimina esse risco.
2. **Esquecer que o sucessor pode ter um filho à direita:** ao religar `pai_suc->esq = suc->dir` (ou `pai_suc->dir = suc->dir`), preservamos toda a subárvore direita do sucessor (como testado com o ID `65` abaixo de `60` ao remover a raiz `50`).
3. **Atualizar `arv->tamanho` só quando a operação realmente teve sucesso:** verificamos `status == BST_OK` na inserção e `removido == 1` na remoção.

---

## 6. Testes Adicionados e Resultado

Criado [tests/unit/test_bst.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/unit/test_bst.c) com 5 casos unitários:
- `deve_criar_arvore_vazia_e_tratar_operacoes`: busca/remoção em árvore vazia e `bst_destruir(NULL)`.
- `deve_inserir_buscar_e_rejeitar_id_duplicado`: inserção, busca existente/inexistente/inválido, rejeição de ID duplicado preservando o registro original.
- `deve_listar_em_ordem_crescente_de_id`: inserção fora de ordem (`40, 20, 60, 10, 30`) e validação da saída em ordem (`10, 20, 30, 40, 60`).
- `deve_remover_no_folha_e_no_com_um_filho`: remoção de folha (0 filhos), nó com 1 filho à direita e nó com 1 filho à esquerda.
- `deve_remover_no_com_dois_filhos_e_raiz`: remoção da raiz `50` com sucessor `60` que possui filho direito `65`, e remoção de subárvore `30` com sucessor imediato `40`, validando a listagem em-ordem resultante.

**Execução (`make clean && make && make test`):**
- Compilação C11 estrita (`-Wall -Wextra -Wpedantic -Werror`): **0 avisos**.
- `test_bst`: `Total: 5 | Passaram: 5 | Falharam: 0`
- Demais suítes (`test_equipamento`, `test_framework`, `test_io`): **100% passando**.
- **Valgrind** (`./build/tests/test_bst`): `55 allocs, 55 frees`, `0 errors from 0 contexts`, `All heap blocks were freed -- no leaks are possible`.

---

## 7. Perguntas de Revisão (para fixação)

1. **Por que a listagem em-ordem da BST sai ordenada sem usar `qsort` ou Merge Sort?**  
   *Resposta:* Porque pela invariante da BST toda a subárvore esquerda tem IDs menores que a raiz e toda a subárvore direita tem IDs maiores. Logo, visitar `esquerda -> raiz -> direita` percorre os IDs em ordem estritamente crescente em tempo $O(n)$.
2. **Na remoção de um nó com 2 filhos, quais são as duas escolhas válidas de substituto?**  
   *Resposta:* O **sucessor em-ordem** (menor valor da subárvore direita, que não tem filho esquerdo) ou o **predecessor em-ordem** (maior valor da subárvore esquerda, que não tem filho direito). Ambos preservam a invariante da BST.
3. **Por que `bst_destruir` não pode usar percurso pré-ordem (`free(no)` antes dos filhos)?**  
   *Resposta:* Porque após `free(no)`, acessar `no->esq` e `no->dir` constitui leitura de memória já liberada (*Use-After-Free*). O percurso pós-ordem garante que os filhos sejam destruídos antes do nó pai.

---

## Commits desta etapa

- `feat(bst): define interface publica bst.h e implementa operacoes da BST`
- `test(bst): cobre insercao, duplicidade, busca, remocoes e destruicao`
- `docs(diario): registra etapa 04 sobre Arvore Binaria de Busca`
