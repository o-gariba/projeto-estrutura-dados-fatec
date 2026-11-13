# Roteiro de Evidências e Testes Reproduzíveis (R08)

Este documento registra os cenários reproduzíveis exigidos pelo projeto, com **entradas, passos, resultados esperados e resultados obtidos**, tanto via testes unitários quanto via testes ponta-a-ponta (E2E) com redirecionamento de entrada e verificação de memória.

---

## Como Reproduzir

```bash
# 1. Compilar e rodar todos os testes (unitarios + E2E)
make clean && make && make test

# 2. Verificar ausencia de vazamentos e erros de memoria (Valgrind)
make memcheck

# 3. Verificar sanitizadores de memoria e comportamento indefinido (ASan/UBSan)
make sanitize
```

Para executar manualmente qualquer cenário E2E específico:
```bash
make
./bin/rede < tests/e2e/casos/01-bst-operacoes.in
```

---

## Matriz de Evidências — Testes Obrigatórios

| Cenário Obrigatório | Arquivo / Teste que Cobre | Entrada e Passos | Resultado Esperado | Resultado Obtido |
|---|---|---|---|---|
| **Inserção, busca e listagem em-ordem na BST** | `tests/e2e/casos/01-bst-operacoes.in` e `tests/unit/test_bst.c` | Listar BST vazia (`4`); cadastrar IDs `50, 30, 70, 20, 40, 60, 80, 65` (`1`); buscar `30` e `99` (`2`); listar em-ordem (`4`). | BST vazia informa mensagem; busca `30` exibe registro e `99` informa não encontrado; listagem imprime `20, 30, 40, 50, 60, 65, 70, 80` em ordem crescente. | **Passou** (`diff` idêntico ao `.esperado`; 7/7 testes unitários da BST OK). |
| **Remoção na BST: folha (0 filhos), 1 filho e 2 filhos** | `tests/e2e/casos/01-bst-operacoes.in` e `tests/unit/test_bst.c` | Remover `20` (folha), `60` (1 filho à direita: `65`), `50` (raiz com 2 filhos -> sucessor `65`), `99` (inexistente) e listar em-ordem (`4`). | Todas as remoções válidas têm sucesso, `99` informa não encontrado, listagem final imprime `30, 40, 65, 70, 80`. | **Passou** (`diff` idêntico ao `.esperado`; inclui caso `pai_suc != no` com `suc->dir != NULL` no unitário). |
| **ID duplicado e ID inválido (`0`, negativo, texto, fora de `int`)** | `tests/e2e/casos/02-entradas-e-duplicidade.in` e `tests/unit/test_io.c` | Cadastrar `ID 10`; tentar cadastrar `0`, `-5`, `abc`, `12abc`, `99999999999` e `10` novamente; testar opções inválidas `9` e `abc`. | Todas as entradas inválidas exibem erro e pedem nova entrada; `10` é rejeitado como duplicado; opção inválida volta ao menu. | **Passou** (`diff` idêntico ao `.esperado`; 8/8 testes unitários de IO OK). |
| **Entradas textuais inválidas (vazia, só espaços, > 79 chars + limpeza de buffer, espaços válidos)** | `tests/e2e/casos/02-entradas-e-duplicidade.in`, `test_io.c` e `test_equipamento.c` | Cadastrar `ID 10` com textos contendo espaços (`"Core Switch 01"`); no `ID 20`, enviar linha vazia, `"   "`, 85 chars `'A'` e depois `"Backup Link"`. | Rejeita vazio, só espaços e > 79 chars; após descartar o excesso, `"Backup Link"` é aceito inteiro sem lixo no buffer e exibido na busca (`2`). | **Passou** (`diff` idêntico ao `.esperado`; 4/4 testes unitários de Equipamento OK). |
| **Lista encadeada: inserção no fim, remoção do início, exibição início→fim e repetição de IDs** | `tests/e2e/casos/03-lista-independente-e-vazia.in` e `tests/unit/test_lista.c` | Inserir `500`, `777`, `500` (repetido e fora da BST), cadastrar `10` na BST, inserir `10` na lista, remover `10` da BST (`3`), exibir lista (`7`), remover do início (`6`) duas vezes e exibir (`7`). | `500 -> 777 -> 500 -> 10` preservado mesmo após excluir `10` da BST; remoções retiram `500` e `777` do início, restando `500 -> 10`. | **Passou** (`diff` idêntico ao `.esperado`; 4/4 testes unitários da Lista OK). |
| **Remoção com estrutura vazia mantendo o programa funcionando** | `tests/e2e/casos/03-lista-independente-e-vazia.in` | Chamar opção `6` logo no início (lista vazia) e novamente após esvaziar todos os elementos da lista; depois encerrar com `0`. | Exibe `"Aviso: estrutura linear vazia. Nada a remover."` nas duas vezes e retorna ao menu sem travar ou encerrar. | **Passou** (`diff` idêntico ao `.esperado`). |
| **Encerramento sem vazamentos de memória (opção `0` e `EOF`)** | `tests/e2e/casos/04-eof-limpeza.in` + todos os testes em `make memcheck` e `make sanitize` | Executar toda a suíte unitária e os 4 fluxos E2E (inclusive interrompendo por `EOF` com dados ainda na BST e na Lista) sob Valgrind e ASan/UBSan. | `0 errors`, `0 leaks` (`All heap blocks were freed`) no Valgrind e saída limpa no ASan/UBSan. | **Passou** (`make memcheck` e `make sanitize` concluídos com código `0`). |
