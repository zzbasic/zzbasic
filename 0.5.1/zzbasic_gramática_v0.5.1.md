---

title: "ZzBasic - Gramática Completa v0.5.1"
css: gramatica-style.css

---

# ZzBasic - Gramática Completa v0.5.1

<div align="center">

**ZzBasic é uma linguagem que combina a simplicidade do BASIC clássico com recursos modernos.**

![Versão](https://img.shields.io/badge/versão-0.5.1-blue)
![Status](https://img.shields.io/badge/status-em_desenvolvimento-yellow)
![Última Atualização](https://img.shields.io/badge/atualização-2026--02--04-green)

</div>

## Índice
- [Estrutura do Programa](#estrutura-do-programa)
- [Statements](#statements)
- [Expressões](#expressões)
- [Literais e Tokens](#literais-e-tokens)
- [Hierarquia de Precedência](#hierarquia-de-precedência)
- [Exemplos](#exemplos)
- [Regras de Associação Importantes](#regras-de-associação-importantes)
- [Notas de Implementação para o Parser](#notas-de-implementação-para-o-parser)

---

## Estrutura do Programa

### **Programa Estrutural**
```
program             := statement_list

statement_list      := statement (EOL statement)* EOL?

statement           := assignment_stmt
                    | print_stmt
                    | color_stmt 
                    | input_stmt 
                    | if_stmt
                    | expression_stmt
```

## Statements

### **Atribuição (Assignment)**
```
assignment_stmt     := 'let' IDENTIFIER '=' expression
```

### **Comando PRINT**
```
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
```

### **Comando COLOR (standalone)**
```
color_stmt          := COLOR_TOKEN
```

### **Comando INPUT**
```
input_stmt          := 'input' (STRING)? IDENTIFIER
```

### **Estrutura IF (Condicional)**
```
if_stmt             := 'if' logical_expr 'then' EOL
                           statement_list
                       ( 'else' 'if' logical_expr 'then' EOL
                           statement_list )*
                       ( 'else' EOL
                           statement_list )?
                       'end' 'if' EOL
```

## Expressões

### **Expressão como Statement**
```
expression_stmt     := logical_expr
```

### **Expressões Lógicas**

```
logical_expr        := logical_or_expr

logical_or_expr     := logical_and_expr ('or' logical_and_expr)*

logical_and_expr    := not_expr ('and' not_expr)*

not_expr            := ('not' | '!')? comparison_expr

comparison_expr     := expression (comparison_op expression)*

comparison_op       := '==' | '!=' | '<' | '>' | '<=' | '>='
```

### **Expressões Aritméticas**

```
expression          := term (('+' | '-') term)*

term                := factor (('*' | '/') factor)*

factor              := ('+' | '-')? atom

atom                := NUMBER 
                    | STRING 
                    | 'true' 
                    | 'false' 
                    | IDENTIFIER 
                    | '(' logical_expr ')'
```

## Literais e Tokens

### **Comentários**
```
COMMENT             := '#' ~[\n\r]*
```

### **Identificadores**
```
IDENTIFIER          := [a-zA-Z_][a-zA-Z0-9_]*
```

### **Números**
```
NUMBER              := [0-9]+ ('.' [0-9]+)?
```

### **Strings**
```
STRING              := '"' [^"]* '"'
```

### **Booleanos**
```
BOOLEAN             := 'true' | 'false'
```

## Palavras-chave

### **Comandos**
```
let    print    input    if    then    else
end    and      or       not   nl
```

### **Cores Normais (8 cores)**
```
black   red     green   yellow
blue    magenta cyan    white
```

### **Cores Bright (8 cores)**
```
bred      bgreen    byellow   bblue
bmagenta  bcyan     bwhite
```

### **Cores de Background (8 cores)**
```
bgblack   bgred     bggreen   bgyellow
bgblue    bgmagenta bgcyan    bgwhite
```

## Operadores

### **Aritméticos**
```
PLUS    := '+'
MINUS   := '-'
MULT    := '*'
DIV     := '/'
```

### **Atribuição**
```
ASSIGN  := '='
```

### **Comparação**
```
EQUAL           := '=='
NOT_EQUAL       := '!='
LESS            := '<'
GREATER         := '>'
LESS_EQUAL      := '<='
GREATER_EQUAL   := '>='
```

### **Parênteses**
```
LPAREN  := '('
RPAREN  := ')'
```

### **Lógicos**
```
AND     := 'and'      # Binário
OR      := 'or'       # Binário
NOT     := 'not' | '!' # Unário
```

## Hierarquia de Precedência

### **Do MAIS FRACO para o MAIS FORTE**

| Nível | Operador | Associatividade | Função do Parser |
|-------|----------|-----------------|------------------|
| **1** | `or` | Esquerda | `parse_logical_or_expr()` |
| **2** | `and` | Esquerda | `parse_logical_and_expr()` |
| **3** | `not`, `!` (unário) | Direita | `parse_not_expr()` |
| **4** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Esquerda | `parse_comparison_expr()` |
| **5** | `+`, `-` (binário) | Esquerda | `parse_expression()` |
| **6** | `*`, `/` | Esquerda | `parse_term()` |
| **7** | `+`, `-` (unário) | Direita | `parse_factor()` |
| **8** | `()`, `true`, `false`, números, strings, variáveis | - | `parse_atom()` |

## Exemplos

### **1. Expressões Booleanas**
```
not x or y and z == 5
(not x) or (y and (z == 5))
```

### **2. Comparações**
```
x > 5 and y <= 10
nome == "João" or idade >= 18
```

### **3. NOT com Múltiplas Formas**
```
not x > 5
! x > 5
not (x > 5)
!(x > 5)
```

### **4. IF Completo**
```
if x > 5 then
    print "Maior que 5" nl
    let y = x * 2
else if x == 5 then
    print "Igual a 5" nl
else
    print "Menor que 5" nl
end if
```

### **5. Expressões Aninhadas**
```
if not (x < 0 or y > 100) and z == 50 then
    print "Condição complexa atendida" nl
end if
```

## Regras de Associação Importantes

### **1. `not` e `!` são Sinônimos**
```
not x == ! x  # São equivalentes
```

### **2. Precedência: `and` > `or`**
```
x or y and z  ==  x or (y and z)
```

### **3. Cuidado com `not` e comparações**
```
not x > 5  ==  (not x) > 5  # Pode não ser intuitivo!
# Para clareza, use:
not (x > 5)
```

### **4. Comparações não podem ser encadeadas**
```
# VÁLIDO:
x > 5 and x < 10

# INVÁLIDO:
5 < x < 10  # Não suportado!
```

### **5. `if` sempre requer `then` na mesma linha**
```
if x > 5 then    # CORRETO
    print "OK"

if x > 5         # ERRADO - falta 'then'
    print "ERRO"

if x > 5         # ERRADO - 'then' em linha separada
    then
    print "ERRO"
```

### **6. Blocos podem ter zero ou mais statements**
```
if x > 5 then
    # Zero statements é válido
end if
```

## Notas de Implementação para o Parser

### **1. Ordem de Implementação (Top-Down)**
Implemente na ordem natural da gramática (do nível mais fraco para o mais forte):
```
parse_logical_or_expr() → Nível 1: operador 'or'
parse_logical_and_expr() → Nível 2: operador 'and'
parse_not_expr() → Nível 3: operadores 'not' e '!'
parse_comparison_expr() → Nível 4: ==, !=, <, >, <=, >=
parse_expression() → Nível 5: +, - (binários)
parse_term() → Nível 6: *, /
parse_factor() → Nível 7: +, - (unários)
parse_atom() → Nível 8: literais, variáveis, parênteses
```
Cada função chama a próxima na hierarquia, criando uma árvore de parsing natural.

### **2. Tratamento de `not` e `!`**
- Ambos devem criar o MESMO nó AST (`NODE_NOT_LOGICAL_OP`)
- São semanticamente equivalentes

### **3. Estrutura para `if`**
Criar uma estrutura que permita:
- Uma condição (`logical_expr`)
- Um bloco `then` (`statement_list`)
- Lista opcional de `else_if` (condição + bloco)
- Bloco `else` opcional (sem condição)

### **4. Diferença entre `=` e `==`**
```
let x = 5    # Atribuição
x == 5       # Comparação
```

## Casos de Borda e Ambiguidades Resolvidas

### **1. NOT vs Negativo Unário**
```
not -5  →  not (-5)    # NOT aplicado ao resultado de -5
!-5     →  ! (-5)      # Equivalente
```

### **2. NOT com Comparação**
```
not x == 5  →  (not x) == 5  # Por precedência
# Para segurança: use parênteses
not (x == 5)
```

### **3. Múltiplos NOT**
```
not not x  →  not (not x)    # Associatividade à direita
!!x        →  !(!x)          # Equivalente
```

### **4. NOT com AND/OR**
```
not x and y  →  (not x) and y
# Para NOT sobre toda a expressão AND:
not (x and y)  # Precisa de parênteses
```

---

<div align="center">

**FIM DA GRAMÁTICA DO ZzBasic v0.5.1**

*Documentação atualizada em 4 de fevereiro de 2026*

arataca89@gmail.com

</div>

