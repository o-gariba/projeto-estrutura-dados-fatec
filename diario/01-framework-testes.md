# Etapa 01: Mini framework de testes

**Requisitos atendidos:** R05 (infraestrutura de testes reproduzíveis), R06 (documentação incremental)  
**Commits:** ver o final deste documento

## Objetivo

Construir um mini framework de testes unitários em C puro (C11) que seja leve, não exija a instalação de bibliotecas externas (como Unity, CUnit ou Criterion) e permita testar cada função e TAD isoladamente, garantindo a reprodutibilidade dos testes exigida no R05 logo desde os primeiros passos.

## Decisões de projeto (e alternativas descartadas)

- **Header-only (`tests/unit/test.h`):** O framework é implementado em um único cabeçalho estático. Isso evita dependências de compilação cruzada ou configuração de bibliotecas extras.
- **Idiom `do { ... } while (0)` nas macros:** Garante segurança sintática. Se uma asserção for usada dentro de um `if (cond) ASSERT(...); else ...`, sem o `do-while(0)` a macro poderia causar problemas graves de interpretação pelo compilador devido ao ponto e vírgula.
- **Stringificação com o operador `#` do pré-processador:** Transforma expressões passadas por parâmetro em literais de string em tempo de compilação. Isso permite que a mensagem de erro exiba exatamente a expressão que falhou (`ASSERT(a == b)` imprime `expressao 'a == b' avaliada como falsa`).
- **Macros `__FILE__` e `__LINE__`:** Mostram o arquivo e a linha precisa da falha, permitindo depuração imediata.
- **Integração limpa com o `Makefile`:** O alvo `make test-unit` descobre automaticamente arquivos que casam com o padrão `tests/unit/test_*.c`, compila cada um vinculando com os objetos compilados da aplicação (`build/*.o`) e executa a suíte.
- **Código de retorno POSIX:** O framework finaliza com `0` se todos os testes passaram e `1` se houve ao menos uma falha. Isso garante que o Makefile (e futuramente o GitHub Actions no CI) detecte falhas imediatamente e interrompa a pipeline.

*Alternativas descartadas:*
- *Bibliotecas externas (Unity/CMocka):* Aumentariam a complexidade de configuração e poderiam dificultar a execução direta em máquinas limpas ou no navegador via WebAssembly.
- *Testes manuais via `printf` no `main`:* Não são automatizados nem reproduzíveis, quebrando o requisito R05.

## Conceitos de Estrutura de Dados e C envolvidos

- **Macros de pré-processador e metaprogramação:** Entender o papel do pré-processador na geração de código antes da compilação.
- **Variáveis estáticas globais (`static` no contexto de arquivo):** Restringem a visibilidade das variáveis contadoras (`g_testes_executados`, `g_testes_falhas`, etc.) ao arquivo de compilação atual.
- **Ponteiros nulos (`NULL`) e comparações seguras:** Como testar referências a ponteiros sem incorrer em falhas de segmentação (*Segmentation Fault*) ao tentar desreferenciar ponteiros nulos em `strcmp`.

## Complexidade das operações

| Operação | Tempo | Espaço |
|---|---|---|
| `ASSERT` / `ASSERT_EQ_INT` | $O(1)$ | $O(1)$ |
| `ASSERT_EQ_STR` | $O(k)$ (onde $k$ é o tamanho da string) | $O(1)$ |
| Execução de teste (`RUN_TEST`) | $O(1)$ overhead | $O(1)$ |

## Dificuldades e soluções

- **Problema de comparador de strings com ponteiro nulo:** Ao usar `strcmp(a, b)`, se qualquer uma das strings for `NULL`, ocorre comportamento indefinido (falha de segmentação).
- **Solução:** Na macro `ASSERT_EQ_STR`, incluímos uma verificação prévia de nulidade: se ambos forem nulos, são considerados equivalentes; se apenas um for nulo, a asserção falha informando os ponteiros sem chamar `strcmp`.

## Testes adicionados e resultado

Criado o arquivo de teste de fumaça `tests/unit/test_framework.c`:
- `deve_passar_em_assert_simples`: valida asserções booleanas verdadeiras.
- `deve_comparar_inteiros`: valida comparação de igualdade numérica.
- `deve_comparar_strings`: valida comparação correta de texto literal.
- `deve_tratar_ponteiros_nulos`: valida checagem de ponteiro nulo e não-nulo.

**Resultado da execução (`make test`):**
```text
Total: 4 | Passaram: 4 | Falharam: 0
```
Verificado também via **Valgrind**: zero erros de memória e nenhum vazamento (0 leaks).

## Perguntas de revisão para a prova (com respostas)

1. **Por que se utiliza a construção `do { ... } while(0)` na definição de macros em C?**  
   *Resposta:* Para transformar múltiplos comandos dentro da macro em uma única instrução sintática composta que aceita ponto e vírgula no final. Isso previne erros de sintaxe e comportamentos inesperados quando a macro é usada em estruturas de controle como `if-else` sem chaves.

2. **O que faz o operador `#` (stringificação) no pré-processador do C?**  
   *Resposta:* O operador `#` converte o argumento passado para a macro em uma constante literal de string (entre aspas), sem avaliar seu valor em tempo de execução. Exemplo: `#cond` se torna `"cond"`.

3. **Qual a importância de um código de saída diferente de zero (`exit(1)`) ao final de uma suíte de testes?**  
   *Resposta:* Sistemas operacionais, scripts de shell, Makefiles e ferramentas de CI (como GitHub Actions) monitoram o código de término do processo ($?$). O valor `0` indica sucesso; qualquer valor diferente de zero indica falha, permitindo que a automação aborte o processo e reporte o erro.

## Commits desta etapa

- `test: adiciona mini framework de testes unitarios`
- `docs(readme): documenta execucao dos testes`
- `docs(diario): registra etapa 01`
