# ZzBasic - Documentação v0.5.4

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

![ZzBasic Logo](zzbasic_logo1.png)

## 📚 Documentação

- [Manual do Usuário](zzbasic_0_5_4_manual.md)
- [Realce de Sintaxe para o Editor Sublime Text](editor-support/)

## Versão Atual: **0.5.4**

### 🚀 Características

*   ✅ **Sintaxe limpa** - Comandos em caixa baixa (`let`, não `LET`)
*   ✅ **Tipagem dinâmica** - Números, strings e booleanos
*   ✅ **REPL interativo avançado** - Com comandos de debug, análise e modo multi-linha
*   ✅ **Execução de arquivos** - Scripts com extensão `.zz`
*   ✅ **Múltiplos statements** - Separe comandos com `:`, `;` ou nova linha
*   ✅ **Verificação de tipos** - Erros claros em operações inválidas
*   ✅ **Gerenciamento de memória** - Sem vazamentos
*   ✅ **Análise léxica e AST** - Ferramentas de debug integradas
*   ✅ **Comando `print`** - Saída formatada com controle explícito de linha
*   ✅ **Atalho `?`** - Alternativa rápida para `print`
*   ✅ **Cores no `print`** - Suporte a 24 cores (normais, brilhantes e fundo)
*   ✅ **Largura de campo (width) e alinhamento (alignment) no `print`**
*   ✅ **Comando `input`** - Entrada de dados do usuário com formatação
*   ✅ **Tipo booleano** - `true` e `false`
*   ✅ **Operações de comparação** - `==`, `!=`, `<`, `>`, `<=`, `>=`
*   ✅ **Operações lógicas** - `and`, `or`, `not`, `!`
*   ✅ **Estrutura `if...else`** - Controle condicional
*   ✅ **Loop `while`** - Repetição com condição
*   ✅ **Loop `for`** - Repetição com contador e step
*   ✅ **Controle de fluxo** - `break` e `continue`
*   ✅ **REPL Multi-linha** - Modo `program` para editar e executar programas
*   ✅ **Comandos REPL** - `list`, `edit`, `delete`, `run`, `purge`

### 📋 Histórico de Versões

| Versão | Status | Recursos Principais |
| --- | --- | --- |
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| 0.4.0 | Lançada | Comando `print`, controle de linha, saída formatada |
| 0.4.1 | Lançada | Comando `print` com cores |
| 0.4.2 | Lançada | Largura de campo e alinhamento no `print` |
| 0.4.3 | Lançada | `input` |
| 0.5.0 | Lançada | Tipo booleano |
| 0.5.1 | Lançada | Operadores de comparação e operadores lógicos |
| 0.5.2 | Lançada | Estrutura `if...else` |
| 0.5.3 | Lançada | Loop `while`; `break`; `continue` |
| 0.5.4 | **Lançada** | Loop `for`; `input` com cores/alinhamento; REPL multi-linha |
| 0.6.0 | Planejada | Funções, Arrays, Sistema de Módulos, Tipo `text` |

## 📥 Instalação e Compilação

### Pré-requisitos

*   **Compilador C**: GCC, Clang ou MSVC
*   **Make** (opcional)
*   **Git** (para clonar)

### Compilação (Linux/macOS)

```shell
# Clone o repositório
git clone https://github.com/zzbasic/zzbasic.git
cd zzbasic/0.5.4

# Compile
make

# Execute
./zzbasic
```

### Compilação (Windows)

```shell
# Com MinGW
cd zzbasic\0.5.4
gcc -o zzbasic *.c
zzbasic.exe
```

## 📚 Referência da Linguagem

### Gramática EBNF (v0.5.4)

```
# =====================================================================
# ZzBasic - GRAMÁTICA v0.5.4
# Loop for; input com formatação; REPL multi-linha
# Última atualização: 20260209
# =====================================================================

# =====================================================================
# PROGRAMA ESTRUTURAL 
# =====================================================================
program             := statement_list

statement_list      := statement (separator+ statement)*

separator           := EOL | ':' | ';'

statement           := assignment_stmt
                    | print_stmt
                    | color_stmt 
                    | input_stmt 
                    | if_stmt
                    | while_stmt
                    | for_stmt
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
input_stmt := 'input' (color_directive)? (width_spec)? (alignment)? (STRING)? (nocolor)? IDENTIFIER


# =====================================================================
# IF STATEMENT
# =====================================================================
if_stmt             := 'if' logical_expr 'then' EOL
                           statement_list
                       ( 'else' 'if' logical_expr 'then' EOL
                           statement_list )*
                       ( 'else' EOL
                           statement_list )?
                       'end' 'if' 


# =====================================================================
# WHILE STATEMENT
# =====================================================================
while_stmt := 'while' '(' logical_expr ')' 'do' EOL
                 statement_list*
              'end' 'while' 


# =====================================================================
# FOR STATEMENT
# =====================================================================
for_stmt := 'for' IDENTIFIER '=' expression 'to' expression ('step' expression)? 'do' EOL
                statement_list
            'end' 'for'


# =====================================================================
# BREAK STATEMENT
# =====================================================================
break_stmt := 'break' EOL

# =====================================================================
# CONTINUE STATEMENT
# =====================================================================
continue_stmt := 'continue' EOL


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
# HIERARQUIA DE PRECEDÊNCIA (do MAIS FRACO para o MAIS FORTE)
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

# 1. Expressões booleanas
#    not x or y and z == 5
#    (not x) or (y and (z == 5))

# 2. Comparações
#    x > 5 and y <= 10
#    nome == "João" or idade >= 18

# 3. NOT com múltiplas formas
#    not x > 5
#    ! x > 5
#    not (x > 5)
#    !(x > 5)

# 4. IF completo
#    if x > 5 then
#        print "Maior que 5" nl
#        let y = x * 2
#    else if x == 5 then
#        print "Igual a 5" nl
#    else
#        print "Menor que 5" nl
#    end if

# 5. Expressões aninhadas
#    if not (x < 0 or y > 100) and z == 50 then
#        print "Condição complexa atendida" nl
#    end if


# =====================================================================
# REGRAS DE ASSOCIAÇÃO IMPORTANTES
# =====================================================================

# 1. 'not' e '!' são SINÔNIMOS e têm a MESMA precedência
#    not x == ! x  (são equivalentes)

# 2. 'and' tem precedência maior que 'or'
#    x or y and z  ==  x or (y and z)

# 3. 'not'/'!' tem precedência maior que comparações
#    not x > 5  ==  (not x) > 5  (CUIDADO! Isso pode não ser intuitivo)
#    Para segurança: use parênteses: not (x > 5)

# 4. Comparações não podem ser encadeadas sem operadores
#    VÁLIDO:    x > 5 and x < 10
#    INVÁLIDO:  5 < x < 10  (não suportado)

# 5. 'if' sempre requer 'then' na mesma linha
#    if x > 5 then  (CORRETO)
#    if x > 5       (ERRADO - falta 'then')
#        then       (ERRADO - 'then' em linha separada)

# 6. Cada bloco (if, else if, else) contém uma statement_list
#    (zero ou mais statements)


# =====================================================================
# NOTAS DE IMPLEMENTAÇÃO PARA O PARSER
# =====================================================================

# 1. Implementar na ordem da precedência (nível 8 para cima):
#    parse_atom() → parse_factor() → parse_term() → parse_expression() →
#    parse_comparison_expr() → parse_not_expr() → parse_logical_and_expr() →
#    parse_logical_or_expr()

# 2. 'not' e '!' devem criar o MESMO nó AST (NODE_NOT_LOGICAL_OP)

# 3. Para o 'if', criar uma estrutura que permita:
#    - Uma condição (logical_expr)
#    - Um bloco then (statement_list)
#    - Lista opcional de else_if (condição + bloco)
#    - Bloco else opcional (sem condição)

# 4. Lembrar que '=' é apenas para atribuição (let x = 5)
#    e '==' é para comparação (x == 5)


# =====================================================================
# CASOS DE BORDA E AMBIGUIDADES RESOLVIDAS
# =====================================================================

# 1. NOT vs negativo unário:
#    - not -5  →  not (-5)    [NOT aplicado ao resultado de -5]
#    - !-5     →  ! (-5)      [equivalente]

# 2. NOT com comparação:
#    - not x == 5  →  (not x) == 5  [por precedência]
#    - Use parênteses para clareza: not (x == 5)

# 3. Múltiplos NOT:
#    - not not x  →  not (not x)    [associatividade à direita]
#    - !!x        →  !(!x)          [equivalente]

# 4. NOT com AND/OR:
#    - not x and y  →  (not x) and y
#    - not (x and y) → precisa de parênteses

# FIM DA GRAMÁTICA DO ZzBasic v0.5.4
# =====================================================================
```

### Palavras-Chave da Linguagem

| Categoria | Palavras-Chave |
|-----------|----------------|
| **Comandos** | `let`, `print`, `input`, `if`, `then`, `else`, `end`, `while`, `do`, `for`, `to`, `step`, `break`, `continue`, `nl` |
| **Operadores Lógicos** | `and`, `or`, `not` |
| **Booleanos** | `true`, `false` |
| **Cores Normais** | `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white` |
| **Cores Brilhantes** | `bred`, `bgreen`, `byellow`, `bblue`, `bmagenta`, `bcyan`, `bwhite` |
| **Cores de Fundo** | `bgblack`, `bgred`, `bggreen`, `bgyellow`, `bgblue`, `bgmagenta`, `bgcyan`, `bgwhite` |
| **Formatação** | `width`, `left`, `right`, `center`, `nocolor` |

### Operadores

| Tipo | Operadores |
|------|-----------|
| **Aritméticos** | `+`, `-`, `*`, `/` |
| **Atribuição** | `=` |
| **Comparação** | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| **Lógicos** | `and`, `or`, `not`, `!` |

## 🛠️ Ferramentas de Debug (REPL)

O REPL do ZzBasic oferece comandos especiais para ajudar no desenvolvimento e depuração:

### Comandos Básicos

| Comando | Descrição |
|---------|-----------|
| `exit` ou `quit` | Sai do REPL |
| `help` ou `?` | Mostra ajuda |
| `clear` | Limpa a tela |

### Comandos de Análise

| Comando | Descrição |
|---------|-----------|
| `vars` | Lista todas as variáveis e seus valores |
| `reset` | Apaga todas as variáveis |
| `tokens "código"` | Mostra os tokens gerados pelo lexer |
| `ast "código"` | Mostra a Árvore de Sintaxe Abstrata (AST) |
| `symbols` | Mostra a tabela de símbolos |

### Modo Multi-linha (Program Mode)

| Comando | Descrição |
|---------|-----------|
| `program` | Entra em modo program para editar múltiplas linhas |
| `end program` | Sai do modo program e compila o código |
| `run` | Executa o programa em memória |
| `list` | Lista todas as linhas do programa |
| `list n` | Lista a linha n |
| `list n-m` | Lista de linha n até linha m |
| `list n-` | Lista de linha n até o fim |
| `list -m` | Lista do início até linha m |
| `edit n` | Edita a linha n |
| `delete n` | Deleta a linha n |
| `delete n-m` | Deleta de linha n até linha m |
| `delete n-` | Deleta de linha n até o fim |
| `delete -m` | Deleta do início até linha m |
| `purge` | Limpa o programa e variáveis da memória |

### Exemplo de Uso do REPL Multi-linha

```
>> program
[PROGRAM MODE]
01: for i = 1 to 10 do
02:     print i nl
03: end for
04: end program
[Program loaded successfully]
>> run
1
2
3
4
5
6
7
8
9
10
>> list 1-2
01: for i = 1 to 10 do
02:     print i nl
>> edit 2
02:     print i nl
02:     print "Número: " i nl
>> run
Número: 1
Número: 2
...
Número: 10
>> purge
[Memory purged]
```

## 📝 Exemplos

### Exemplo 1: Programa Simples

```zzbasic
print "Bem-vindo ao ZzBasic!" nl
print "Digite seu nome: "
input nome
print "Olá, " nome nl
```

### Exemplo 2: Loop com Contador

```zzbasic
for i = 1 to 10 do
    print i nl
end for
```

### Exemplo 3: Loop com Step

```zzbasic
for i = 1 to 30 step 2 do
    print i ", "
end for
print nl
```

### Exemplo 4: Estrutura Condicional

```zzbasic
input "Digite um número: " numero

if (numero > 0) then
    print "Número positivo" nl
else if (numero < 0) then
    print "Número negativo" nl
else
    print "Número é zero" nl
end if
```

### Exemplo 5: Loop While

```zzbasic
let i = 1
while (i <= 10) do
    print i nl
    let i = i + 1
end while
```

### Exemplo 6: INPUT com Formatação

```zzbasic
input cyan width(40) center "Entre com seu nome: " nocolor nome
input green "Digite sua idade: " idade
print "Nome: " nome ", Idade: " idade nl
```

### Exemplo 7: Jogo de Adivinhação

```zzbasic
let numero_secreto = 7
let tentativas = 0

while (tentativas < 3) do
    input "Adivinhe o número (1-10): " chute
    let tentativas = tentativas + 1
    
    if (chute == numero_secreto) then
        print green "Parabéns! Você acertou!" nocolor nl
        break
    else if (chute < numero_secreto) then
        print yellow "Muito baixo, tente novamente!" nocolor nl
    else
        print red "Muito alto, tente novamente!" nocolor nl
    end if
end while

if (tentativas == 3) then
    print red "Fim do jogo! O número era " numero_secreto nocolor nl
end if
```

## 🗺️ Roadmap

### v0.6.0 (Planejado)

- ✅ Funções e escopo (global/local)
- ✅ Arrays dinâmicos (híbridos: stack/heap)
- ✅ Sistema de módulos (import/from)
- ✅ Tipo `text` (strings dinâmicas)
- ✅ Módulo `math` (sqrt, abs, sin, cos, etc.)
- ✅ Módulo `string` (upper, lower, substr, etc.)
- ✅ Módulo `io` (read, write, append)

### v0.7.0 (Futuro)

- Struct (tipos definidos pelo usuário)
- Enum (enumerações)
- Módulo `list` (listas com métodos)
- Módulo `dict` (dicionários)
- Classes (orientação a objetos)

## 📜 Licença

Este projeto é licenciado sob a Licença MIT. Veja o arquivo LICENSE para mais detalhes.

---

**Desenvolvido com ❤️ por arataca89**

**Contato:** arataca89@gmail.com
