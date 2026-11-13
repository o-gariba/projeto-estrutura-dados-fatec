# Etapa 07: Testes Reproduzíveis (E2E + Evidências) e Memória

**Requisitos atendidos:** R07 (execução contínua após erro e 0 vazamentos ao encerrar), R08 (testes reproduzíveis com entradas, passos, esperado e obtido)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Completar a pirâmide de testes do projeto adicionando **testes ponta-a-ponta (E2E — *End-to-End*)** sobre o executável real `./bin/rede`, automatizar a checagem de memória com **Valgrind (`make memcheck`)** e **AddressSanitizer / UndefinedBehaviorSanitizer (`make sanitize`)**, e consolidar o roteiro de evidências em [tests/ROTEIRO_EVIDENCIAS.md](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/ROTEIRO_EVIDENCIAS.md).

---

## 2. Ideia Central e Modelo Mental

Até a Etapa 06, tínhamos **testes unitários** (*caixa branca*): cada função de `io`, `equipamento`, `bst` e `lista` era testada isoladamente. Mas testar peças isoladas não garante que:
- Os prompts do menu estejam alinhados com o fluxo esperado;
- O `stdin` compartilhado entre menu e prompts se comporte corretamente após erros;
- O binário final libere toda a memória quando o usuário interage e encerra pela opção `0` ou por `EOF`.

Os **testes E2E** (*caixa preta*) fecham essa lacuna executando `./bin/rede` como um usuário real:
```text
arquivo .in  ──(redirecionamento <)──►  ./bin/rede  ──►  saída obtida
                                                             │
arquivo .esperado  ◄─────────────── diff -u ─────────────────┘
```
Se qualquer caractere da saída mudar por regressão futura, `diff -u` aponta a linha exata da divergência e falha o `make test`.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Runner E2E em shell (`tests/e2e/run_e2e.sh`)

- Usa `set -euo pipefail` para interromper imediatamente em erros inesperados.
- Para cada `tests/e2e/casos/*.in`, redireciona a entrada para `./bin/rede`, grava a saída em arquivo temporário (`mktemp`) e compara com o `.esperado` correspondente usando `diff -u`.
- Remove arquivos temporários e sai com código `1` se qualquer cenário divergir.

### 3.2. `make memcheck` (Valgrind Memcheck)

Flag por flag do comando adicionado ao `Makefile`:
- `--leak-check=full`: detalha cada bloco alocado e não liberado e a pilha de chamadas onde o `malloc` aconteceu.
- `--show-leak-kinds=all`: verifica não apenas `definitely lost`, mas também `indirectly lost`, `possibly lost` e `still reachable`.
- `--error-exitcode=1`: faz o Valgrind retornar `1` se achar qualquer erro de memória (leitura inválida, uso após `free`, vazamento), permitindo automatizar a validação.
- `-q`: suprime o cabeçalho informativo quando não há erros, mantendo a saída limpa.

### 3.3. `make sanitize` (ASan + UBSan)

Compila com `-fsanitize=address,undefined -fno-omit-frame-pointer`:
- **AddressSanitizer (ASan):** detecta estouro de buffer na stack/heap, *use-after-free* e vazamentos em tempo de execução.
- **UndefinedBehaviorSanitizer (UBSan):** detecta estouro de inteiro com sinal, deslocamentos inválidos, ponteiros nulos desreferenciados etc.
- Ao final do alvo `sanitize`, rodamos `$(MAKE) clean` para que o diretório `build/` não fique com objetos instrumentados misturados com o build normal.

### 3.4. Caso `04-eof-limpeza.in`

Esse cenário insere dados tanto na BST quanto na Lista e **interrompe o fluxo no meio de um cadastro via `EOF`** (sem digitar `0`). Rodá-lo sob Valgrind comprova que a liberação de memória funciona mesmo em encerramentos abruptos do fluxo de entrada.

---

## 4. Complexidade e Custo da Suíte de Verificação

| Verificação | Escopo | Resultado |
|---|---|---|
| `make test-unit` | 23 testes unitários em 5 suítes | 23/23 passando |
| `make test-e2e` | 4 cenários completos sobre `./bin/rede` | 4/4 idênticos ao esperado |
| `make memcheck` | Valgrind nos 5 binários unitários + 4 casos E2E | 0 erros, 0 leaks |
| `make sanitize` | ASan + UBSan em todos os testes unitários e E2E | 0 erros, 0 UB |

---

## 5. Armadilhas Comuns e Como Evitamos

1. **Testar apenas o "caminho feliz" nos E2E:** incluímos entradas malformadas no meio de cadastros (`02-entradas-e-duplicidade.in`) e remoção em lista vazia antes e depois de usar a lista (`03-lista-independente-e-vazia.in`).
2. **Esquecer objetos instrumentados com ASan no `build/`:** resolvido executando `$(MAKE) clean` ao final de `make sanitize`.
3. **Diferenças de quebra de linha ou falta de `fflush`:** evitado com `fflush(out)` após prompts em `app.c`, garantindo ordem determinística entre prompts e saídas em redirecionamentos.

---

## 6. Perguntas de Revisão (para fixação)

1. **Qual a diferença entre teste unitário e teste E2E neste projeto?**  
   *Resposta:* O teste unitário valida funções específicas de um único TAD (ex.: `bst_remover` ou `io_ler_id`) diretamente em código C; o teste E2E executa o programa compilado `./bin/rede` de ponta a ponta simulando um usuário real via redirecionamento de entrada e comparando toda a saída com `diff`.
2. **O que o Valgrind quer dizer com `definitely lost` vs `still reachable`?**  
   *Resposta:* `definitely lost` indica memória alocada cujo último ponteiro foi perdido (impossível dar `free`, vazamento real); `still reachable` indica memória que ainda tinha um ponteiro apontando para ela ao encerrar, mas não foi liberada com `free` antes do `return`. Com `--show-leak-kinds=all`, garantimos zero blocos em ambas as categorias.
3. **Por que usar ASan/UBSan além do Valgrind?**  
   *Resposta:* O Valgrind atua em tempo de execução sobre memória dinâmica (heap) e instruções de máquina; já o ASan/UBSan são instrumentados pelo compilador e conseguem pegar também estouros de arrays locais na **stack** (como `char buf[80]`) e **comportamento indefinido** em C (como overflow de inteiro com sinal).

---

## Commits desta etapa

- `test(e2e): adiciona casos reproduziveis cobrindo todos os testes minimos`
- `build: adiciona alvos test-e2e, memcheck e sanitize ao Makefile`
- `docs(tests): registra roteiro de evidencias com entradas e resultados`
- `docs(diario): registra etapa 07 sobre testes reproduziveis e memoria`
