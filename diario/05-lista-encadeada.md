# Etapa 05: Lista Encadeada Simples (`lista.h` e `lista.c`)

**Requisitos atendidos:** R04 (estrutura linear dinâmica própria), R05 (IDs positivos independentes da BST, permitindo repetidos), R06 (inserir no fim, remover do início, exibir do início ao fim), R07 (tratar remoção em estrutura vazia e liberar memória), R09 (modularização em `.h`/`.c`)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Implementar a estrutura linear dinâmica própria do sistema — uma **Lista Encadeada Simples** em [include/lista.h](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/include/lista.h) e [src/lista.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/src/lista.c) — para armazenar IDs inteiros positivos de forma totalmente independente da BST.

---

## 2. Ideia Central e Modelo Mental

Em um vetor dinâmico (`int *v`), os elementos ficam contíguos na memória: remover do início custa $O(n)$ (porque todos os elementos seguintes precisam ser deslocados) e crescer exige `realloc`.

Na **Lista Encadeada Simples**:
- Cada elemento vive em seu próprio bloco na heap (`NoLista`), contendo `int id` e um ponteiro `struct NoLista *prox` para o próximo nó;
- O descritor `Lista` guarda **dois ponteiros de extremidade** (`inicio` e `fim`) além de `tamanho`:
  ```text
  Lista { inicio ──► [10 | •] ──► [20 | •] ──► [10 | NULL] ◄── fim }
  ```
- Inserir no fim e remover do início acontecem em **tempo constante $O(1)$**, mexendo apenas nos ponteiros das pontas, sem percorrer os $n$ elementos nem deslocar dados na memória.

### Independência em relação à BST (R05)

A lista **não** guarda `Equipamento *`, nem consulta a BST:
- Aceita IDs que não existem na BST;
- Aceita IDs repetidos (ex.: `42 -> 7 -> 42`);
- Se um equipamento for removido da BST, seu ID continua intacto na lista até ser explicitamente removido pelo usuário ou até `lista_destruir()`.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Por que manter o ponteiro `fim` no descritor?

Sem o ponteiro `fim`, `lista_inserir_fim()` precisaria caminhar desde `inicio` até encontrar `atual->prox == NULL`, custando $O(n)$ a cada inserção. Com `fim`:
- Se `lista->fim == NULL` (lista vazia): `inicio = novo` e `fim = novo`.
- Caso contrário: `lista->fim->prox = novo` e `lista->fim = novo`.

Isso reduz a inserção no fim de $O(n)$ para **$O(1)$**.

### 3.2. O caso crítico de `lista_remover_inicio()` ao esvaziar a lista

Ao remover o primeiro nó (`alvo = lista->inicio`):
```c
lista->inicio = alvo->prox;
if (lista->inicio == NULL) {
    lista->fim = NULL;
}
free(alvo);
```
Se a lista tinha **apenas 1 elemento**, `alvo->prox` é `NULL`, logo `lista->inicio` passa a ser `NULL`. Se esquecêssemos de fazer `lista->fim = NULL`, o ponteiro `fim` continuaria apontando para a memória recém-liberada por `free(alvo)` — um ***dangling pointer***! Na próxima inserção, `lista->fim->prox = novo` escreveria em memória inválida (corrupção de heap / *Segmentation Fault*).

### 3.3. Remoção segura em lista vazia (R07)

Quando `lista->inicio == NULL`, `lista_remover_inicio()` retorna `LISTA_ERRO_VAZIA` sem alterar `tamanho` nem `id_removido`. A camada de aplicação (`app.c`) lê esse status, informa o usuário e mantém o programa em execução normalmente.

### 3.4. Salvar `prox` antes de chamar `free(atual)` em `lista_destruir()`

Na destruição da lista:
```c
while (atual != NULL) {
    NoLista *prox = atual->prox;
    free(atual);
    atual = prox;
}
```
Se avançássemos com `atual = atual->prox` *depois* de `free(atual)`, estaríamos lendo memória já devolvida ao alocador (*Use-After-Free*). Salvar `prox` antes do `free` evita esse erro clássico.

---

## 4. Complexidade das Operações

| Operação | Tempo | Espaço | Observação |
|---|---|---|---|
| `lista_criar` | $O(1)$ | $O(1)$ | Aloca apenas o descritor `Lista` |
| `lista_inserir_fim` | $O(1)$ | $O(1)$ | Acesso direto via ponteiro `fim` |
| `lista_remover_inicio` | $O(1)$ | $O(1)$ | Acesso direto via ponteiro `inicio` |
| `lista_exibir` | $O(n)$ | $O(1)$ | Percorre os $n$ nós do início ao fim |
| `lista_vazia` / `lista_tamanho` | $O(1)$ | $O(1)$ | Consulta campo mantido no descritor |
| `lista_destruir` | $O(n)$ | $O(1)$ | Percorre e libera cada nó iterativamente |

---

## 5. Armadilhas Comuns e Como Evitamos

1. ***Dangling pointer* em `fim` ao remover o último elemento:** resolvido com `if (lista->inicio == NULL) lista->fim = NULL;` e validado pelo teste `deve_inserir_fim_remover_inicio_e_reinserir_apos_esvaziar`.
2. **Bloquear IDs duplicados por hábito:** na BST a unicidade é obrigatória (R03), mas na lista encadeada a repetição é **permitida** (R05). Mantivemos as regras estritamente separadas e testadas.
3. **Vazamento quando o usuário encerra sem esvaziar a lista:** `lista_destruir()` percorre todos os nós remanescentes e garante `0` bytes vazados ao sair.

---

## 6. Testes Adicionados e Resultado

Criado [tests/unit/test_lista.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/unit/test_lista.c) com 3 casos unitários:
- `deve_criar_lista_vazia_e_tratar_remocao_vazia`: verifica estado inicial e que remover da lista vazia retorna `LISTA_ERRO_VAZIA` sem alterar a lista.
- `deve_rejeitar_ids_nao_positivos_e_permitir_repetidos`: rejeita `0` e `-10`, insere `42`, `7`, `42` e verifica exibição exata `"42 -> 7 -> 42\n"`.
- `deve_inserir_fim_remover_inicio_e_reinserir_apos_esvaziar`: testa inserção única seguida de remoção (validando `inicio == NULL` e `fim == NULL`), reinserção múltipla, remoção parcial e destruição com nó restante.

**Execução (`make clean && make && make test`):**
- Compilação C11 estrita (`-Wall -Wextra -Wpedantic -Werror`): **0 avisos**.
- `test_lista`: `Total: 3 | Passaram: 3 | Falharam: 0`
- Demais suítes (`test_bst`, `test_equipamento`, `test_framework`, `test_io`): **100% passando**.
- **Valgrind** (`./build/tests/test_lista`): `13 allocs, 13 frees`, `0 errors from 0 contexts`, `All heap blocks were freed -- no leaks are possible`.

---

## 7. Perguntas de Revisão (para fixação)

1. **Qual o ganho de guardar um ponteiro `fim` além de `inicio` na `struct Lista`?**  
   *Resposta:* Permite inserir no final em tempo $O(1)$ (acesso direto ao último nó) em vez de percorrer toda a lista desde `inicio` em $O(n)$.
2. **Por que a lista ainda pode causar *Segmentation Fault* se esquecermos `lista->fim = NULL` ao remover o único nó?**  
   *Resposta:* Porque `fim` continuará apontando para o bloco liberado por `free(alvo)`. Na próxima inserção, `lista->fim->prox = novo` tentará escrever em memória liberada (*Use-After-Free*).
3. **Se `lista_remover_inicio` é $O(1)$, quanto custaria uma operação `lista_remover_fim` em uma lista encadeada simples (sem ponteiro `ant`)?**  
   *Resposta:* Custaria $O(n)$, porque mesmo tendo `fim`, precisaríamos percorrer a lista desde `inicio` para descobrir o **penúltimo** nó e atualizar `fim` e `penultimo->prox = NULL`. (Uma lista duplamente encadeada resolveria isso em $O(1)$ ao custo de um ponteiro extra por nó.)

---

## Commits desta etapa

- `feat(lista): implementa lista encadeada simples de IDs positivos`
- `test(lista): cobre insercao no fim, remocao do inicio, vazia e duplicados`
- `docs(diario): registra etapa 05 sobre lista encadeada`
