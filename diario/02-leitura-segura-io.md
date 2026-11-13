# Etapa 02: Alinhamento de Escopo, Correção do Framework e Leitura Segura (`io`)

**Requisitos atendidos:** R01 (validação de entradas), R03 (rejeição de IDs e textos inválidos), R05 (validação numérica compartilhada), R08 (testes unitários reproduzíveis)  
**Commits:** ver o fim deste arquivo

---

## 1. Objetivo

Construir um módulo reutilizável de entrada segura (`include/io.h` e `src/io.c`) que isole toda a lógica de leitura de texto e conversão de números inteiros do restante da aplicação. Em C, entradas de usuário malformadas são a fonte mais comum de *buffer overflow*, leitura "pulada" (sujeira deixada no `stdin`) e comportamento indefinido.

Aproveitamos também esta etapa para:
1. Alinhar o `README.md` com os requisitos **R01–R10** definidos para o tema **REDES** (BST + Lista Encadeada Simples, sem o campo `ano`).
2. Corrigir um bug silencioso em `tests/unit/test.h`: a macro `RUN_TEST` não incrementava `g_testes_falhas` quando um teste falhava, o que faria `TEST_RESUMO()` retornar `0` mesmo com asserções falsas.

---

## 2. Ideia Central e Modelo Mental

Em vez de espalhar `scanf` ou `fgets` pelos módulos de menu e estruturas de dados, tratamos a entrada em **duas camadas bem separadas**:

1. **Camada `io` (ler e classificar):** recebe um fluxo `FILE *` (no programa real é `stdin`; nos testes unitários é um arquivo temporário `tmpfile()`), lê **sempre uma linha inteira** e devolve um código de status (`IoStatus`) dizendo exatamente o que aconteceu (`IO_OK`, `IO_ERRO_VAZIO`, `IO_ERRO_LONGO`, `IO_ERRO_FORMATO`, `IO_ERRO_INTERVALO`, `IO_EOF`).
2. **Camada `app` (interagir):** decide que mensagem mostrar e se pede a entrada de novo.

Essa separação traz duas vantagens diretas:
- **Testabilidade:** o módulo `io` pode ser testado automaticamente sem interação humana, usando `tmpfile()`.
- **Consistência:** tanto o cadastro na BST (R03) quanto a inserção na Lista Encadeada (R05) chamam exatamente a mesma função `io_ler_id`, garantindo a mesma regra de validação numérica.

---

## 3. Detalhes Técnicos e Decisões de Projeto

### 3.1. Por que `fgets` + buffer interno de 81 bytes (`TAM_TEXTO + 1`)?

O requisito determina campos textuais de até **79 caracteres úteis**, armazenados em `char[80]` (`79` chars + `'\0'`).

Aqui existe uma armadilha clássica de *off-by-one*: se passássemos diretamente um buffer de `80` bytes para `fgets(buf, 80, in)`, a função leria no máximo `79` caracteres e deixaria o `\n` (que o usuário digitou ao apertar Enter) preso no buffer de entrada! Ao verificar `strchr(buf, '\n') == NULL`, uma entrada perfeitamente válida de **79 caracteres** seria falsamente rejeitada como longa demais.

**Solução adotada em `io_ler_linha`:**
- Usamos um buffer temporário interno `char tmp[TAM_TEXTO + 1]` (**81 bytes**): comporta `79` caracteres úteis + `'\n'` + `'\0'`.
- Se `strchr(tmp, '\n') != NULL`: removemos o `'\n'` (e eventual `'\r'` de quebras CRLF) e copiamos para `buf[80]`. A entrada coube no limite e o fluxo ficou limpo.
- Se `strchr(tmp, '\n') == NULL`:
  - Se `strlen(tmp) < TAM_TEXTO` (`< 80`), o fluxo terminou em `EOF` logo após um texto de até 79 caracteres — caso válido!
  - Se `strlen(tmp) == TAM_TEXTO` (`80` caracteres sem `\n`), o usuário digitou **80 ou mais caracteres úteis**. Chamamos `descartar_resto_linha(in)` (um laço `fgetc` até `'\n'` ou `EOF`) e retornamos `IO_ERRO_LONGO`.

### 3.2. Por que `strtol` em vez de `scanf("%d")` ou `atoi()`?

| Função | Problema |
|---|---|
| `scanf("%d", &x)` | Quando o usuário digita `"abc"`, `scanf` falha e **deixa `"abc\n"` no `stdin`**, causando loop infinito se houver `while`. Além disso, `12abc` é aceito parcialmente como `12`, e overflow gera comportamento indefinido. |
| `atoi("abc")` | Retorna `0` sem distinguir `"0"` de `"abc"`, e tem comportamento indefinido em overflow. |
| `strtol(buf, &endptr, 10)` | Permite detectar **todos** os erros de conversão com precisão determinística. |

**Checklist usado em `io_ler_inteiro_intervalo`:**
1. Lemos a linha com `io_ler_linha` e rejeitamos vazio/só espaços (`IO_ERRO_VAZIO`).
2. Zeramos `errno = 0` antes de chamar `strtol(buf, &endptr, 10)`.
3. Se `endptr == buf`, nenhum dígito foi consumido (ex.: `"abc"`, `"+"`) → `IO_ERRO_FORMATO`.
4. Avançamos `endptr` apenas por espaços em branco (`isspace`). Se sobrar qualquer caractere diferente de `'\0'` (ex.: `"12abc"` ou `"12 34"`) → `IO_ERRO_FORMATO`.
5. Verificamos `errno == ERANGE || valor < INT_MIN || valor > INT_MAX`. Esse passo duplo é essencial em sistemas Linux 64 bits (LP64), onde `long` tem 64 bits e `int` tem 32 bits: o número `"99999999999"` cabe em `long` (não dispara `ERANGE`), mas **estoura `int`**! Assim garantimos validação estrita para `int` em qualquer arquitetura.
6. Verificamos `valor < min || valor > max` → `IO_ERRO_INTERVALO`.

### 3.3. Cast `(unsigned char)` nas funções de `<ctype.h>`

Em C, funções como `isspace(int c)` esperam um valor representável como `unsigned char` ou `EOF`. Passar um `char` assinado com bit de sinal ligado (como caracteres acentuados em UTF-8) gera **comportamento indefinido** (*Undefined Behavior*). O cast `(unsigned char)s[i]` elimina esse risco.

---

## 4. Complexidade das Operações

| Operação | Tempo | Espaço | Observação |
|---|---|---|---|
| `io_ler_linha` | $O(k)$ | $O(1)$ | $k$ = tamanho real da linha digitada (descarta excesso em streaming, sem alocar heap) |
| `io_texto_valido` | $O(k)$ | $O(1)$ | Limite máximo verificado de 79 chars |
| `io_ler_texto` | $O(k)$ | $O(1)$ | Apenas buffers fixos na stack |
| `io_ler_inteiro_intervalo` | $O(k)$ | $O(1)$ | Buffer fixo de 80 bytes na stack |

---

## 5. Armadilhas Comuns e Como Evitamos

1. **Buffer sujo após entrada > 79 caracteres:** evitado com `descartar_resto_linha(in)`, comprovado pelo teste `deve_rejeitar_texto_maior_que_79_chars_e_limpar_buffer`, onde a leitura imediatamente seguinte devolve `"Switch Sala 2"` intacta.
2. **Overflow silencioso de `int` em sistemas 64 bits:** evitado checando `valor < INT_MIN || valor > INT_MAX` além de `errno == ERANGE`.
3. **Bug na contagem de falhas de `test.h`:** corrigido `else { g_testes_falhas++; }` em `RUN_TEST` para garantir que nenhuma falha passe despercebida no código de saída.

---

## 6. Testes Adicionados e Resultado

Criado [tests/unit/test_io.c](file:///home/pedro-garibaldi-santos/Documents/fatec/estrutura-de-dados/projeto-pratico-p2/tests/unit/test_io.c) com 7 casos unitários usando `tmpfile()`:
- `deve_rejeitar_texto_vazio_e_so_espacos`
- `deve_aceitar_texto_com_espacos_e_exatamente_79_chars`
- `deve_rejeitar_texto_maior_que_79_chars_e_limpar_buffer`
- `deve_aceitar_texto_79_chars_sem_newline_no_eof`
- `deve_validar_id_positivo_e_rejeitar_invalidos` (`""`, `"   "`, `"abc"`, `"12abc"`, `"12 34"`, `"0"`, `"-5"`, `"99999999999"`, overflow extremo, `"  42  "`, `INT_MAX`, `EOF`)
- `deve_validar_opcoes_de_menu`
- `deve_validar_funcao_io_texto_valido`

**Execução (`make clean && make && make test`):**
- Compilação com `-std=c11 -Wall -Wextra -Wpedantic -Werror`: **0 avisos**.
- `test_framework`: `Total: 4 | Passaram: 4 | Falharam: 0`
- `test_io`: `Total: 7 | Passaram: 7 | Falharam: 0`
- **Valgrind** (`valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./build/tests/test_io`): `0 errors from 0 contexts`, `13 allocs, 13 frees`, `All heap blocks were freed -- no leaks are possible`.

---

## 7. Perguntas de Revisão (para fixação)

1. **Por que ler a entrada como string (`fgets`) e converter depois (`strtol`) é mais seguro do que `scanf("%d")`?**  
   *Resposta:* `fgets` sempre consome a linha (e permite drenar o restante se ultrapassar o limite), evitando que tokens inválidos fiquem travados no `stdin` em loops de leitura. Já `strtol` expõe `endptr` e `errno`, permitindo rejeitar lixo sufixado (`12abc`) e overflow (`99999999999`).
2. **Por que o buffer interno de `io_ler_linha` tem `TAM_TEXTO + 1` (81 bytes) se o campo do registro tem `char[80]`?**  
   *Resposta:* Porque `fgets(tmp, sizeof(tmp), in)` lê no máximo `sizeof(tmp) - 1` caracteres. Com 81 bytes, ele lê 79 caracteres úteis + `'\n'` numa só chamada, distinguindo com exatidão 79 caracteres (válido) de 80+ caracteres (inválido).
3. **Por que checar `errno == ERANGE` sozinho não basta para detectar overflow de `int` no Linux 64 bits?**  
   *Resposta:* No modelo LP64, `long` possui 64 bits e `int` possui 32 bits. Valores entre $2^{31}$ e $2^{63}-1$ cabem em `long` sem ativar `ERANGE`, mas não cabem em `int`. Portanto, é obrigatório comparar também com `INT_MIN` e `INT_MAX`.

---

## Commits desta etapa

- `fix(test): contabiliza falhas na macro RUN_TEST`
- `docs(readme): alinha tabela de requisitos R01-R10 com a proposta`
- `feat(io): implementa leitura segura de texto, buffer e inteiros`
- `test(io): cobre entradas invalidas, overflow e limpeza de buffer`
- `docs(diario): registra etapa 02 sobre leitura segura`
