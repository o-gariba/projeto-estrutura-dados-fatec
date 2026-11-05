# Etapa 00: Esqueleto e build reproduzível

**Requisitos atendidos:** R02 (estrutura base), R06 (README inicial)
**Commits:** ver o fim deste arquivo

## Objetivo

Criar a estrutura de pastas, um `Makefile` que compila tudo com um único comando e um `main.c` mínimo. A meta é que qualquer pessoa consiga rodar `git clone` e `make run` sem configurar nada.

## Decisões de projeto

- **`include/` separado de `src/`.** Separa a *interface* (o que o TAD oferece, nos `.h`) da *implementação* (como ele faz, nos `.c`). Esse é o princípio central de um TAD: quem usa não precisa saber como funciona por dentro.
- **Compilação em objetos (`.o`).** Cada `.c` é compilado separadamente para `build/`. O `main.c` fica fora da lista `OBJ`, para que os testes (Etapa 1) possam ligar os mesmos objetos com outro `main`.
- **Flags rígidas (`-Wall -Wextra -Wpedantic -Werror`).** Avisos costumam indicar bugs reais em C, como variável não inicializada ou formato errado no `printf`. Tratá-los como erro força a corrigir na hora.
- **`CC = gcc` em vez de `CC ?= gcc`.** O `make` já define `CC=cc` por padrão, então o `?=` (atribuir só se não definida) nunca teria efeito. Ainda dá para trocar o compilador com `make CC=clang`, porque variáveis passadas na linha de comando têm prioridade.
- **Alvo `help` autodocumentado.** Cada alvo tem um comentário `## descrição`, e o `make help` extrai esses comentários com `grep` e `awk`. Assim a documentação nunca fica desatualizada em relação ao Makefile.
- **Menu sem acentos.** Evita problemas de codificação em terminais diferentes (a proposta permite isso).

**Alternativas descartadas:** CMake (mais poderoso, mas é mais uma ferramenta para instalar e aprender) e compilar tudo num único comando `gcc src/*.c` (simples, mas recompila tudo sempre e não reaproveita os objetos nos testes).

## Conceitos envolvidos

- **Compilação separada:** pré-processamento → compilação (`.c` → `.o`) → ligação (`.o` → executável).
- **Header (`.h`):** declara funções e tipos; o `.c` os define.

## Testes

Verificação manual:
```bash
make clean && make && make run
```
Resultado: compilou sem avisos e imprimiu a mensagem inicial. Um segundo `make` respondeu "Nada a ser feito", o que confirma a recompilação incremental.

## Perguntas de revisão

1. **Qual a diferença entre declarar e definir uma função?**
   Declarar (no `.h`) informa a assinatura: nome, parâmetros e retorno. Definir (no `.c`) fornece o corpo. Uma função pode ser declarada várias vezes, mas definida uma única vez.
2. **Por que separar `.h` e `.c` num TAD?**
   Para esconder a implementação. Quem usa o TAD inclui só o `.h` e depende apenas da interface. A implementação pode mudar sem quebrar quem a usa.
3. **O que `-Werror` faz e por que é útil?**
   Converte avisos em erros de compilação. Em C, muitos avisos apontam comportamento indefinido em potencial.

## Commits desta etapa

- `build: adiciona Makefile e estrutura inicial`
- `chore: ignora artefatos de compilação`
- `docs(readme): cria README com instruções de execução`
- `docs(diario): registra etapa 00`
