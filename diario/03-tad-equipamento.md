# Etapa 03: TAD `equipamento` (`struct`, invariantes e ciclo de vida)

**Requisitos atendidos:** R01 (`struct`, ponteiros e funções), R03 (invariantes de ID e textos de até 79 caracteres), R09 (modularização em `.h`/`.c`)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Modelar o registro central do domínio **REDES** (`Equipamento`: `id`, `nome`, `tipo`, `localizacao`) como um **Tipo Abstrato de Dados (TAD)** com ciclo de vida bem definido (`equipamento_criar`, `equipamento_imprimir`, `equipamento_destruir`) e **validação defensiva** de todas as invariantes no momento da construção.

---

## 2. Ideia Central e Modelo Mental

Um **TAD** não é apenas uma `struct`: é uma `struct` acompanhada de **garantias (invariantes)** mantidas por suas funções.

Se permitíssemos que qualquer parte do código alocasse um `Equipamento` manualmente com `malloc` e preenchesse os campos diretamente sem validação, poderíamos acabar com um registro inválido dentro da BST (por exemplo, `id = -3` ou `nome = ""`). Ao centralizar a criação em `equipamento_criar()`, estabelecemos uma **fronteira de confiança**:

> Todo `Equipamento *` retornado por `equipamento_criar()` garante por construção que `id > 0` e que `nome`, `tipo` e `localizacao` têm entre 1 e 79 caracteres úteis, contendo ao menos um caractere não branco.

Além disso, alocar `Equipamento` dinamicamente na **heap** (em vez de copiar a `struct` inteira por valor a cada inserção/remoção na árvore) permite que a BST manipule apenas **ponteiros** (`Equipamento *`), evitando cópias repetidas de `3 * 80 = 240` bytes de texto durante operações como remoção com dois filhos.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Campos em `char[80]` vs comprimento máximo `79`

Cada campo textual tem `TAM_TEXTO` (`80`) bytes na `struct`:
- Índices `0..78`: até **79 caracteres úteis**;
- Índice `79`: reservado para o terminador nulo `'\0'`.

Por que `memcpy(destino, origem, len + 1)` em vez de `strncpy` ou `strcpy`?
- `strcpy` é inseguro se a entrada não tiver sido validada antes.
- `strncpy` não garante `'\0'` ao final se a string tiver exatamente o tamanho do buffer e preenche o restante do array com zeros desnecessários.
- Como `io_texto_valido(origem)` já garantiu previamente `strlen(origem) <= 79`, `memcpy(destino, origem, len + 1)` copia exatamente os caracteres e o `'\0'` em uma operação clara e livre de avisos de truncamento do GCC.

### 3.2. `sizeof(*eq)` em vez de `sizeof(Equipamento)`

No `malloc`, escrevemos:
```c
eq = malloc(sizeof(*eq));
```
Essa forma idiomática em C evita discrepâncias caso o tipo da variável `eq` mude no futuro e não exige *cast* no ponteiro `void *` retornado por `malloc` (em C11, `void *` converte implicitamente para qualquer ponteiro de objeto).

### 3.3. Validação defensiva (*Defense in Depth*)

Mesmo sabendo que o módulo `io` (Etapa 02) validará o que o usuário digita no terminal, `equipamento_criar()` volta a validar todos os parâmetros usando `io_texto_valido()` e `id <= 0`. Isso impede que chamadas diretas feitas por testes ou novos módulos quebrem as regras do domínio.

### 3.4. `const` correctness e destruição segura com `NULL`

- `equipamento_imprimir(const Equipamento *eq, FILE *out)` recebe `const Equipamento *`: o compilador impede qualquer modificação acidental no equipamento durante a impressão.
- `equipamento_destruir(Equipamento *eq)` chama `free(eq)`. Pelo padrão C11, `free(NULL)` é garantidamente uma operação segura (*no-op*), simplificando rotinas de limpeza em casos de erro.

---

## 4. Complexidade das Operações

| Operação | Tempo | Espaço | Observação |
|---|---|---|---|
| `equipamento_criar` | $O(1)$ | $O(1)$ | `strlen` limitada a no máximo 79 caracteres (constante) e 1 alocação fixa (`sizeof(Equipamento)`) |
| `equipamento_destruir` | $O(1)$ | $O(1)$ | Libera 1 bloco na heap |
| `equipamento_imprimir` | $O(1)$ | $O(1)$ | Impressão formatada de campos de tamanho limitado |

---

## 5. Armadilhas Comuns e Como Evitamos

1. **Alocar `Equipamento` na stack e guardar o endereço na BST:** causaria *dangling pointer* assim que a função retornasse. Aqui, `equipamento_criar` sempre aloca na **heap** e transfere a posse (*ownership*) do ponteiro para quem chamou (a BST, na Etapa 04).
2. **Esquecer de validar ponteiros `NULL`:** passar `NULL` em `nome`, `tipo`, `localizacao` ou `out` não causa *Segmentation Fault*, pois todas as funções verificam `NULL` antes de desreferenciar.
3. **Incluir campo `ano` indevidamente:** conforme definido no escopo do tema **REDES**, o campo `ano` pertence somente ao tema *Biblioteca*; mantê-lo fora de `Equipamento` mantém o modelo enxuto e fiel ao tema.

---

## 6. Testes Adicionados e Resultado

Criado [tests/unit/test_equipamento.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/unit/test_equipamento.c) com 4 casos unitários:
- `deve_criar_equipamento_valido_e_imprimir`: verifica todos os campos e a formatação de saída via `tmpfile()`.
- `deve_rejeitar_id_invalido`: testa `id = 0` e `id = -7`.
- `deve_rejeitar_campos_textuais_invalidos`: testa `NULL`, `""`, só espaços (`"   \t "`, `" \n "`) e strings de 80 caracteres em cada um dos 3 campos.
- `deve_aceitar_exatamente_79_caracteres_e_destruir_null`: valida limite exato de 79 caracteres e segurança com `NULL`.

**Execução (`make clean && make && make test`):**
- Compilação estrita C11 (`-Wall -Wextra -Wpedantic -Werror`): **0 avisos**.
- `test_equipamento`: `Total: 4 | Passaram: 4 | Falharam: 0`
- `test_framework`: `Total: 4 | Passaram: 4 | Falharam: 0`
- `test_io`: `Total: 7 | Passaram: 7 | Falharam: 0`
- **Valgrind** (`./build/tests/test_equipamento`): `5 allocs, 5 frees`, `0 errors from 0 contexts`, `All heap blocks were freed -- no leaks are possible`.

---

## 7. Perguntas de Revisão (para fixação)

1. **Por que a BST guardará `Equipamento *` (ponteiros) em vez de armazenar a `struct Equipamento` por valor em cada nó?**  
   *Resposta:* Guardar ponteiros evita copiar 240+ bytes de strings durante trocas de nós (como na remoção com 2 filhos) e separa claramente a responsabilidade de criação/validação (`equipamento.c`) da responsabilidade de indexação/busca (`bst.c`).
2. **Qual a diferença entre o tamanho do vetor `char nome[80]` e o máximo de 79 caracteres úteis?**  
   *Resposta:* Em C, toda string precisa terminar com o byte nulo `'\0'`. Portanto, um array de 80 bytes comporta no máximo 79 caracteres visíveis mais 1 byte para o `'\0'`.
3. **O que significa `const Equipamento *eq` na assinatura de `equipamento_imprimir`?**  
   *Resposta:* Significa que a função recebe um ponteiro para um `Equipamento` somente leitura: ela pode ler `eq->id`, `eq->nome`, etc., mas o compilador rejeita qualquer tentativa de alterar os campos através de `eq`.

---

## Commits desta etapa

- `feat(equipamento): implementa TAD Equipamento com validacao de invariantes`
- `test(equipamento): cobre criacao valida e rejeicao de campos invalidos`
- `docs(diario): registra etapa 03 sobre TAD e alocacao dinamica`
