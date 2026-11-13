# Etapa 08: Consolidação Técnica e Base para o Relatório em PDF

**Requisitos atendidos:** R08 (consolidação de evidências), R10 (README final e base técnica estruturada para o relatório em PDF)  
**Commits:** ver o fim deste arquivo

---

## 1. Visão Geral do Sistema Final

O sistema implementado resolve o gerenciamento de **equipamentos de rede** (`id`, `nome`, `tipo`, `localizacao`) em C11 estrito (`-std=c11 -Wall -Wextra -Wpedantic -Werror`), combinando duas estruturas de dados dinâmicas independentes orquestradas por um menu de terminal:

1. **Árvore Binária de Busca (`bst.h` / `bst.c`):** armazena `Equipamento *` ordenados por `id` inteiro positivo e único (`1..INT_MAX`), oferecendo inserção, busca, remoção nos 3 casos estruturais (0, 1 e 2 filhos) e listagem em-ordem crescente.
2. **Lista Encadeada Simples (`lista.h` / `lista.c`):** armazena IDs inteiros positivos independentemente da BST, permitindo IDs fora da BST, IDs repetidos e persistência mesmo após exclusão na BST, com inserção no fim em $O(1)$, remoção do início em $O(1)$ e exibição do início ao fim em $O(m)$.
3. **Leitura Segura (`io.h` / `io.c`):** valida comprimentos até 79 caracteres úteis em `char[80]`, rejeita strings vazias ou só com espaços, drena sobras de buffer em entradas longas e converte inteiros com `strtol` + `errno` + `endptr` + limites de `int`.

---

## 2. Roteiro Pronto para o Relatório Técnico (PDF — R10)

As seções abaixo resumem os pontos técnicos que devem compor o relatório final em PDF:

### 2.1. Introdução e Tema
- Problema: cadastrar e consultar equipamentos de rede pelo identificador único `id` via BST, mantendo paralelamente uma lista encadeada dinâmica de IDs positivos independente da árvore.
- Restrições: C11 puro, sem bibliotecas externas, sem vazamentos de memória, tratamento completo de entradas inválidas.

### 2.2. Arquitetura Modular (TADs)
- **`io` (`include/io.h`, `src/io.c`):** leitura segura baseada em `fgets` (buffer interno de 81 bytes para suportar 79 chars + `\n` + `\0`), drenagem do `stdin` em `IO_ERRO_LONGO`, e `strtol` com checagem de `ERANGE` e `[INT_MIN, INT_MAX]`.
- **`equipamento` (`include/equipamento.h`, `src/equipamento.c`):** `struct Equipamento` com validação defensiva de invariantes em `equipamento_criar()` e liberação em `equipamento_destruir()`.
- **`bst` (`include/bst.h`, `src/bst.c`):** separação obrigatória (R09); detém posse (*ownership*) dos `Equipamento *` inseridos com sucesso; remoção de 2 filhos usa transferência do ponteiro do sucessor em-ordem e desconexão direta do nó estrutural do sucessor (sem *double free*); destruição em pós-ordem.
- **`lista` (`include/lista.h`, `src/lista.c`):** descritor com `inicio`, `fim` e `tamanho`; `lista_inserir_fim` em $O(1)$; `lista_remover_inicio` em $O(1)$ atualizando `fim = NULL` ao esvaziar e retornando `LISTA_ERRO_VAZIA` sem falhar quando vazia.
- **`app` (`include/app.h`, `src/app.c`) e `main.c`:** implementa exatamente o Menu de Referência (`0` a `7`), propaga `IO_EOF` sem loops infinitos e garante `bst_destruir` + `lista_destruir` em todo encerramento.

### 2.3. Complexidade Algorítmica
- **BST:** inserção, busca e remoção em $O(h)$ ($O(\log n)$ em árvore balanceada, $O(n)$ no pior caso degenerado); listagem em-ordem e destruição em $O(n)$.
- **Lista Encadeada Simples:** inserção no fim e remoção no início em $O(1)$; exibição e destruição em $O(m)$.

### 2.4. Evidências Reproduzíveis de Teste e Memória
Referenciar diretamente a tabela de [tests/ROTEIRO_EVIDENCIAS.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/ROTEIRO_EVIDENCIAS.md) e os resultados obtidos:
- **Testes unitários (`make test-unit`):** 23/23 passando (`test_framework`: 4, `test_io`: 7, `test_equipamento`: 4, `test_bst`: 5, `test_lista`: 3).
- **Testes ponta-a-ponta (`make test-e2e`):** 4/4 cenários idênticos ao esperado (`diff -u`).
- **Valgrind (`make memcheck`):** 0 erros e 0 bytes vazados (`All heap blocks were freed`) em todos os binários unitários e cenários E2E (incluindo `04-eof-limpeza.in`).
- **Sanitizadores (`make sanitize`):** compilação e execução completa sob `-fsanitize=address,undefined` com 0 erros.

---

## 3. Mapa Completo do Diário de Estudo

| Etapa | Arquivo | Conceito Central Aprendido |
|---|---|---|
| 00 | [00-esqueleto.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/00-esqueleto.md) | Compilação separada (`.c` → `.o` → binário) e Makefile com `-Werror` |
| 01 | [01-framework-testes.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/01-framework-testes.md) | Macros `do { ... } while (0)`, stringificação (`#`) e testes em C puro |
| 02 | [02-leitura-segura-io.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/02-leitura-segura-io.md) | `fgets` com descarte de excesso, `strtol` (`errno`/`endptr`) e `tmpfile()` |
| 03 | [03-tad-equipamento.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/03-tad-equipamento.md) | TAD, alocação dinâmica na heap e validação defensiva de invariantes |
| 04 | [04-bst.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/04-bst.md) | BST, 3 casos de remoção (0, 1, 2 filhos), sucessor em-ordem e pós-ordem |
| 05 | [05-lista-encadeada.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/05-lista-encadeada.md) | Ponteiros `inicio`/`fim`, inserção/remoção $O(1)$ e prevenção de *dangling pointer* |
| 06 | [06-aplicacao-menu.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/06-aplicacao-menu.md) | Separação em camadas, `EOF` seguro e posse determinística de recursos |
| 07 | [07-testes-e-memoria.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/diario/07-testes-e-memoria.md) | Testes E2E com `diff`, Valgrind Memcheck e sanitizadores ASan/UBSan |
| 08 | `08-consolidacao-relatorio.md` | Consolidação final, tabela de complexidade e roteiro para o relatório PDF |

---

## Commits desta etapa

- `docs(readme): consolida documentacao final, complexidade e reproducao de testes`
- `docs(diario): registra etapa 08 como base do relatorio tecnico`
