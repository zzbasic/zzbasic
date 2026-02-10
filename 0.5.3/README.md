# ZzBasic - Documentação v0.5.3

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

* [Manual do Usuário](zzbasic_0_5_3_manual.md)

* [Realce de Sintaxe para o Editor Sublime Text](https://github.com/zzbasic/zzbasic/tree/main/editor-support/sublime-text)

## Versão Atual: **0.5.3**

### 🚀 Características

- ✅ **Sintaxe limpa** - Comandos em caixa baixa (`let`, não `LET`)
- ✅ **Tipagem dinâmica** - Números e strings
- ✅ **REPL interativo avançado** - Com comandos de debug e análise
- ✅ **Execução de arquivos** - Scripts com extensão `.zz`
- ✅ **Múltiplos statements** - Separe comandos com `:`, `;` ou nova linha
- ✅ **Verificação de tipos** - Erros claros em operações inválidas
- ✅ **Gerenciamento de memória** - Sem vazamentos
- ✅ **Análise léxica e AST** - Ferramentas de debug integradas
- ✅ **Comando `print`** - Saída formatada com controle explícito de linha
- ✅ **Atalho `?`** - Alternativa rápida para `print`
- ✅ **Cores no `print`** - Suporte a 24 cores (normais, brilhantes e fundo)
- ✅ **Largura de campo(width) e alinhamento(alignment) no `print`** 
- ✅ **Comando `input`** - Entrada de dados do usuário
- ✅ **Tipo booleano** - `true` e `false`
- ✅ **Operações de comparação** - `==` , `!=` , `<` , `>` , `<=` , `>=`
- ✅ **Operações lógicas** - `and`, `or`, `not`, `!`
- ✅ **Estrutura** `if...else`
- ✅ **Loop** `while`
- ✅ **Controle de fluxo** `break` e `continue`


**OBSERVAÇÃO**: Não implementa comparação de strings, somente de números e booleanos.

---

### 📋 Histórico de Versões

| Versão | Status | Recursos Principais |
|--------|--------|-------------------|
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| 0.4.0 | Lançada | Comando `print`, controle de linha, saída formatada |
| 0.4.1 | Lançada | Comando `print` com cores |
| 0.4.2 | Lançada | Largura de campo e alinhamento no `print` |
| 0.4.3 | Lançada | `input` |
| 0.5.0 | Lançada | tipo booleano |
| 0.5.1 | Lançada | operadores de comparação e operadores lógicos|
| 0.5.2 | Lançada | estrutura `if...else` |
| 0.5.3 | Lançada | loop `while`; `break`; `continue` |
| 0.5.4 | **em desenvolvimento** | loop `for`; `input` com cores/alinhamento; REPL multi-linha |

---

## 📥 Instalação e Compilação

### Pré-requisitos

- **Compilador C**: GCC, Clang ou MSVC
- **Make** (opcional)
- **Git** (para clonar)

### Compilação (Linux/macOS)

```bash
# Clone o repositório
git clone https://github.com/zzbasic/zzbasic.git
cd zzbasic/0.5.3

# Compile
make

# Execute
./zzbasic
```

### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.5.3
gcc -o zzbasic *.c
zzbasic.exe
```

---

## 📚 Referência da Linguagem

### Gramática EBNF (v0.5.3)

```
# =====================================================================
# ZzBasic - GRAMÁTICA COMPLETA v0.5.3
# Estruturas de Controle: while, break, continue
# Última atualização: 20260208
# =====================================================================

# =====================================================================
# PROGRAMA ESTRUTURAL 
# =====================================================================
program             := statement_list

statement_list      := statement (EOL statement)* EOL?

statement           := assignment_stmt
                    | print_stmt
                    | color_stmt 
                    | input_stmt 
                    | if_stmt
                    | while_stmt
                    | break_stmt
                    | continue_stmt
                    | expression_stmt


# =====================================================================
# ASSIGNMENT
# =====================================================================
assignment_stmt     := 'let' IDENTIFIER '=' expression


# =====================================================================
# PRINT STATEMENT
# =====================================================================
print_stmt          := ('print' | '?') print_item* ('nl' | EOL | EOF)

print_item          := expression 
                    | format_directive
                    | color_directive

format_directive    := alignment
                    | width_spec

alignment           := 'left' | 'right' | 'center'

width_spec          := 'width' '(' NUMBER ')'

color_directive     := color_name 
                    | 'nocolor'

color_name          := normal_color
                    | bright_color
                    | background_color

normal_color        := 'black' | 'red' | 'green' | 'yellow'
                    | 'blue' | 'magenta' | 'cyan' | 'white'

bright_color        := 'bred' | 'bgreen' | 'byellow' | 'bblue'
                    | 'bmagenta' | 'bcyan' | 'bwhite'

background_color    := 'bgblack' | 'bgred' | 'bggreen' | 'bgyellow'
                    | 'bgblue' | 'bgmagenta' | 'bgcyan' | 'bgwhite'


# =====================================================================
# COLOR STATEMENT (standalone)
# =====================================================================
color_stmt          := COLOR_TOKEN


# =====================================================================
# INPUT STATEMENT
# =====================================================================
input_stmt          := 'input' (STRING)? IDENTIFIER  


# =====================================================================
# IF STATEMENT
# =====================================================================
if_stmt             := 'if' logical_expr 'then' EOL
                           statement_list
                       ( 'else' 'if' logical_expr 'then' EOL
                           statement_list )*
                       ( 'else' EOL
                           statement_list )?
                       'end' 'if' EOL


# =====================================================================
# WHILE STATEMENT
# =====================================================================
while_stmt          := 'while' '(' logical_expr ')' 'do' EOL
                         statement_list
                       'end' 'while' EOL


# =====================================================================
# BREAK & CONTINUE
# =====================================================================
break_stmt          := 'break'
continue_stmt       := 'continue'


# =====================================================================
# EXPRESSIONS (Hierarquia completa)
# =====================================================================

# Statement expression (top-level)
expression_stmt     := logical_expr

# Logical expressions

# Nível 1: OR (mais baixa precedência)
logical_expr        := logical_or_expr

logical_or_expr     := logical_and_expr ('or' logical_and_expr)*

# Nível 2: AND
logical_and_expr    := not_expr ('and' not_expr)*

# Nível 3: NOT (unário)
not_expr            := ('not' | '!')? comparison_expr

# Nível 4: Comparações
comparison_expr     := expression (comparison_op expression)*

comparison_op       := '==' | '!=' | '<' | '>' | '<=' | '>='

# Nível 5: Expressões aritméticas
expression          := term (('+' | '-') term)*

# Nível 6: Termos (multiplicação/divisão)
term                := factor (('*' | '/') factor)*

# Nível 7: Fatores (unários e átomos)
factor              := ('+' | '-')? atom

# Nível 8: Átomos (mais alta precedência)
atom                := NUMBER 
                    | STRING 
                    | 'true' 
                    | 'false' 
                    | IDENTIFIER 
                    | '(' logical_expr ')'


# =====================================================================
# LITERALS
# =====================================================================
COMMENT             := '#' ~[\n\r]*

IDENTIFIER          := [a-zA-Z_][a-zA-Z0-9_]*

NUMBER              := [0-9]+ ('.' [0-9]+)?

STRING              := '"' [^"]* '"'

BOOLEAN             := 'true' | 'false'


# =====================================================================
# KEYWORDS
# =====================================================================
# Comandos
let
print
input
if
then
else
end
and
or
not
nl
while
do
break
continue

# Cores normais
black
red
green
yellow
blue
magenta
cyan
white

# Cores bright
bred
bgreen
byellow
bblue
bmagenta
bcyan
bwhite

# Cores background
bgblack
bgred
bggreen
bgyellow
bgblue
bgmagenta
bgcyan
bgwhite


# =====================================================================
# OPERATORS
# =====================================================================
# Aritméticos
PLUS                := '+'
MINUS               := '-'
MULT                := '*'
DIV                 := '/'

# Atribuição
ASSIGN              := '='

# Comparação
EQUAL               := '=='
NOT_EQUAL           := '!='
LESS                := '<'
GREATER             := '>'
LESS_EQUAL          := '<='
GREATER_EQUAL       := '>='

# Parênteses
LPAREN              := '('
RPAREN              := ')'

# Lógicos (binários)
AND                 := 'and'
OR                  := 'or'

# Lógicos (unários)
NOT                 := 'not' | '!'


# =====================================================================
# HIERARQUIA DE PRECEDÊNCIA 
# 1 - MENOR PRECEDÊNCIA
# 8 - MAIOR PRECEDÊNCIA
# =====================================================================
Nível   Operador                Associatividade     Função do Parser
---------------------------------------------------------------------
1       or                      Esquerda            parse_logical_or_expr()
2       and                     Esquerda            parse_logical_and_expr()
3       not, ! (unário)         Direita             parse_not_expr()
4       ==, !=, <, >, <=, >=    Esquerda            parse_comparison_expr()
5       +, - (binário)          Esquerda            parse_expression()
6       *, /                    Esquerda            parse_term()
7       +, - (unário)           Direita             parse_factor()
8       (), true, false,        -                   parse_atom()
        números, strings, vars


# =====================================================================
# EXEMPLOS VÁLIDOS
# =====================================================================

# Atribuição
let x = 10
let nome = "Zurg"
let ativo = true

# Expressões
let y = (x + 5) * 2
let z = x > 5 and y < 30

# Print
print "Olá Mundo" nl
print "Idade: " 25 nl
print red "Erro" nocolor nl

# Input
input "Digite seu nome: " nome

# If/Else
if (x > 5) then
    print "Maior que 5" nl
else
    print "Menor ou igual a 5" nl
end if

# While
let i = 0
while (i < 5) do
    print i nl
    let i = i + 1
end while

# Break e Continue
let j = 0
while (j < 10) do
    if (j == 5) then
        break
    end if
    print j nl
    let j = j + 1
end while
```

---

## 🛠️ Ferramentas de Debug (REPL)

O REPL do ZzBasic oferece comandos especiais para ajudar no desenvolvimento e depuração:

| Comando | Descrição |
|---------|-----------|
| `exit` ou `quit` | Sai do REPL |
| `help` ou `?` | Mostra ajuda (futuro) |
| `vars` | Lista todas as variáveis e seus valores |
| `reset` | Apaga todas as variáveis |
| `clear` | Limpa a tela |
| `tokens "código"` | Mostra os tokens gerados pelo lexer |
| `ast "código"` | Mostra a Árvore de Sintaxe Abstrata (AST) |
| `symbols` | Mostra a tabela de símbolos |


---

## 📜 Licença

Este projeto é licenciado sob a Licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

---

arataca89@gmail.com

