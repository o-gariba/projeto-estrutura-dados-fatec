# Etapa 06: Menu Interativo e Integração (`app` e `main`)

**Requisitos atendidos:** R01 (menu interativo, funções e ponteiros), R02 (BST integrada), R03 (rejeição de entradas inválidas e ID duplicado), R04/R05/R06 (Lista Encadeada independente), R07 (aviso em lista vazia e liberação total de memória ao encerrar)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Conectar todos os TADs construídos até aqui (`io`, `equipamento`, `bst` e `lista`) por meio da camada de aplicação ([include/app.h](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/include/app.h) e [src/app.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/src/app.c)), apresentando o **Menu de Referência (`0` a `7`)** e deixando [src/main.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/src/main.c) com responsabilidade única de iniciar a execução com `stdin` e `stdout`.

---

## 2. Ideia Central e Modelo Mental

Um erro comum em projetos em C é colocar `printf`, `fgets` e manipulação direta de ponteiros da árvore/lista todos misturados dentro de um `main()` gigante de 400 linhas. Quando isso acontece, qualquer bug de memória ou mudança de regra exige mexer em tudo ao mesmo tempo.

Aqui usamos **separação em camadas**:
1. **`io`:** só lê e classifica strings/inteiros. Não conhece `BST`, `Lista` nem `Equipamento`.
2. **`bst` e `lista`:** só gerenciam suas estruturas de dados e memória. Não fazem `printf` de menus nem leem do teclado.
3. **`app`:** é o "maestro" (orquestrador). Exibe o menu, chama `io` para obter entradas válidas, chama `bst` ou `lista` conforme a opção escolhida e traduz o status de retorno em mensagens claras para o usuário.
4. **`main`:** apenas chama `app_executar(stdin, stdout)`.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Por que `app_executar(FILE *in, FILE *out)` recebe os fluxos por parâmetro?

Em vez de fixar `stdin` e `stdout` dentro de `app.c`, recebê-los como parâmetros `FILE *in` e `FILE *out` torna possível tanto rodar o programa real (`app_executar(stdin, stdout)`) quanto simular sessões completas em testes automatizados usando `tmpfile()` ou redirecionamento.

### 3.2. Verificação antecipada de ID duplicado na opção 1

No fluxo de cadastro (`tratar_cadastrar_bst`):
1. Lemos o `id` com `solicitar_id()`.
2. Chamamos **`bst_buscar(arv, id)` imediatamente** antes de pedir `nome`, `tipo` e `localizacao`.
3. Se o ID já existir, avisamos na hora e voltamos ao menu — melhorando a usabilidade e evitando alocar `Equipamento` à toa.
4. Mesmo assim, mantemos a checagem de `bst_inserir(arv, eq) != BST_OK` com `equipamento_destruir(eq)` como proteção extra contra vazamentos.

### 3.3. Propagação limpa de `IO_EOF` (Ctrl+D / fim de arquivo)

Em programas de terminal, o usuário pode encerrar enviando `EOF` (`Ctrl+D` no Linux) ou o arquivo de teste E2E pode terminar antes de digitar `0`.
Se um laço `for (;;)` dentro de `solicitar_id()` ou `solicitar_texto()` não tratasse `IO_EOF`, o programa entraria em **loop infinito** imprimindo mensagens de erro!

Por isso, todas as funções auxiliares de leitura retornam `0` em `IO_EOF` e `1` em sucesso:
- Quando qualquer leitura encontra `IO_EOF`, `executando` recebe `0`;
- O laço principal do menu termina;
- O ponto único de saída chama `bst_destruir(arv)` e `lista_destruir(lista)` antes de retornar `EXIT_SUCCESS`.

Assim, **qualquer caminho de encerramento** (opção `0` ou `EOF` no meio de qualquer prompt) libera 100% da memória alocada.

### 3.4. Independência entre BST e Lista no menu (R05)

- **Opção 5 (`tratar_inserir_linear`):** usa exatamente a mesma função `solicitar_id()` (baseada em `io_ler_id`), aplicando a mesma validação numérica da BST, porém **sem consultar a BST** — aceita IDs inexistentes na BST e IDs repetidos.
- **Opção 3 (`tratar_remover_bst`):** remove o equipamento apenas da BST. Se o mesmo ID estiver na lista encadeada, ele permanece lá intacto.
- **Opção 6 (`tratar_remover_linear`):** se a lista estiver vazia (`LISTA_ERRO_VAZIA`), exibe aviso e retorna ao menu mantendo o programa ativo (R07).

---

## 4. Complexidade por Opção do Menu

| Opção | Operação Principal | Complexidade de Tempo |
|---|---|---|
| `1` | `bst_buscar` + `bst_inserir` | $O(h)$ ($O(\log n)$ média, $O(n)$ pior caso) |
| `2` | `bst_buscar` | $O(h)$ |
| `3` | `bst_remover` | $O(h)$ |
| `4` | `bst_listar_em_ordem` | $O(n)$ |
| `5` | `lista_inserir_fim` | $O(1)$ |
| `6` | `lista_remover_inicio` | $O(1)$ |
| `7` | `lista_exibir` | $O(m)$ ($m$ = tamanho da lista) |
| `0` | `bst_destruir` + `lista_destruir` | $O(n + m)$ |

---

## 5. Armadilhas Comuns e Como Evitamos

1. **Loop infinito em `EOF` dentro de prompts de entrada:** evitado retornando `0` em `IO_EOF` e propagando até o encerramento com destruição das estruturas.
2. **Buffer de `stdout` não exibido antes da leitura em pipes/terminais:** evitado com `fflush(out)` logo após imprimir cada prompt.
3. **Esquecer de destruir `Equipamento *` se `bst_inserir` falhar:** evitado chamando `equipamento_destruir(eq)` no ramo `if (st != BST_OK)`.

---

## 6. Verificação Realizada nesta Etapa

- **Compilação e unitários (`make clean && make && make test`):** todos os 23 testes unitários (5 suítes) passaram sem nenhum aviso do compilador (`-Werror`).
- **Teste integrado sob Valgrind** com sequência de cadastro na BST (`10`), listagem em-ordem, inserção na lista de ID inexistente na BST (`999`) e existente (`10`), remoção do ID `10` na BST, exibição da lista (`999 -> 10`), 3 remoções na lista (a 3ª com lista vazia exibindo aviso) e saída pela opção `0`:
  - Resultado Valgrind: `8 allocs, 8 frees`, `0 errors from 0 contexts`, `All heap blocks were freed -- no leaks are possible`.

---

## 7. Perguntas de Revisão (para fixação)

1. **Por que `main.c` fica com apenas uma linha chamando `app_executar(stdin, stdout)`?**  
   *Resposta:* Para isolar o ponto de entrada do sistema operacional da lógica interativa. Isso permite manter a regra do `Makefile` que exclui `src/main.c` ao ligar os testes unitários e possibilita testar o fluxo do menu passando `FILE *` alternativos.
2. **O que aconteceria se um programa em C usasse `while (1)` lendo do teclado e recebesse `Ctrl+D` (`EOF`) sem verificar o retorno de `fgets`?**  
   *Resposta:* `fgets` continuaria retornando `NULL` imediatamente a cada iteração sem esperar entrada do usuário, travando o programa em um loop infinito (e possivelmente consumindo 100% de CPU e enchendo a saída de mensagens de erro).
3. **Por que a opção 1 chama `bst_buscar` antes de ler nome, tipo e localização?**  
   *Resposta:* Para detectar ID duplicado o mais cedo possível (melhor experiência de uso) e evitar alocar/desalocar um `Equipamento` desnecessariamente.

---

## Commits desta etapa

- `feat(app): cria menu de referencia e laco principal com EOF seguro`
- `feat(app): integra operacoes da BST e da lista encadeada`
- `docs(diario): registra etapa 06 sobre integracao e menu`
