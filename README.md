# Rede de Equipamentos e Conexões

Aplicação de terminal em **C** para cadastrar equipamentos de rede (roteadores, switches, servidores...), conectá-los entre si e consultá-los. É um projeto prático da disciplina de **Estrutura de Dados** (FATEC) e aplica, na prática:

- **TADs** (Tipos Abstratos de Dados) separados em `.h` / `.c`
- **Ponteiros** e gerenciamento manual de memória
- **Lista encadeada**, **pilha** e **fila** implementadas do zero
- **Algoritmos de ordenação** (Merge Sort e Insertion Sort, comparados)

Cada equipamento tem `id` (inteiro positivo e único), `nome`, `tipo` e `localização` (textos de até 79 caracteres).

> 📓 O passo a passo do desenvolvimento, com decisões, conceitos e perguntas de revisão, está na pasta [`diario/`](diario/).

---

## Como executar

**Pré-requisitos:** `gcc` e `make`. No Ubuntu/Debian, instale com `sudo apt install build-essential`.

```bash
git clone git@github.com:o-gariba/projeto-estrutura-dados-fatec.git
cd projeto-estrutura-dados-fatec
make run
```

### Comandos disponíveis

| Comando | O que faz |
|---|---|
| `make` (ou `make all`) | Compila a aplicação, gerando o executável `bin/rede` |
| `make run` | Compila (se necessário) e executa |
| `make clean` | Apaga os arquivos gerados (`build/` e `bin/`) |
| `make help` | Lista todos os comandos disponíveis |

---

## Entendendo os comandos

### Flags do compilador (`CFLAGS` no Makefile)

| Flag | Significado |
|---|---|
| `-std=c11` | Segue o padrão **C11** da linguagem, sem extensões específicas do compilador. Garante que o código compile em qualquer compilador C moderno. |
| `-Wall` | Liga os avisos mais comuns (*warnings all*): variável não usada, `printf` com formato errado etc. |
| `-Wextra` | Liga avisos extras, além do `-Wall`: parâmetro não usado, comparação entre `signed` e `unsigned` etc. |
| `-Wpedantic` | Avisa sobre qualquer construção fora do padrão ISO C. |
| `-Werror` | Transforma todo aviso em **erro**, para que nenhum aviso passe despercebido. |
| `-g` | Inclui informações de depuração (nomes de variáveis, linhas), usadas pelo `gdb` e pelo `valgrind`. |
| `-Iinclude` | Diz ao compilador para procurar os arquivos `.h` também na pasta `include/`. |
| `-c` | Só compila (gera o objeto `.o`), sem ligar num executável. |
| `-o <arquivo>` | Define o nome do arquivo de saída. |

### Como o Makefile funciona

A compilação acontece em duas fases:

1. **Compilação:** cada `src/X.c` vira um objeto `build/X.o` (regra `build/%.o: src/%.c`).
2. **Ligação (*linking*):** os objetos são unidos no executável `bin/rede`.

Como cada `.c` é compilado separadamente, o `make` só recompila o que mudou. Os mesmos objetos também serão reaproveitados pelos testes.

Variáveis automáticas usadas no Makefile:

| Variável | Significado |
|---|---|
| `$@` | O alvo da regra (ex.: `bin/rede`) |
| `$<` | A primeira dependência (ex.: `src/main.c`) |
| `$^` | Todas as dependências |

---

## Estrutura do projeto

```
.
├── include/    # Arquivos .h: a interface pública de cada TAD
├── src/        # Arquivos .c: implementação dos TADs e main.c
├── tests/      # Testes automatizados
├── diario/     # Diário de desenvolvimento (uma página por etapa)
└── Makefile    # Automação de compilação, execução e testes
```

---

## Requisitos do projeto

| Código | Requisito | Status |
|---|---|---|
| R01 | Modelagem com `struct`, ponteiros, IDs válidos e leitura segura de entradas | ⬜ |
| R02 | TADs separados em `.h` e `.c`, com gerenciamento correto de memória | ⬜ |
| R03 | Lista encadeada própria com inserção no fim | ⬜ |
| R04 | Pilha dinâmica encadeada: push, pop, peek e consulta das últimas 5 ações | ⬜ |
| R05 | Testes reproduzíveis, tratamento de entradas inválidas e de casos-limite | ⬜ |
| R06 | Repositório Git com evolução contínua, README e relatório técnico em PDF | 🟨 em andamento |
