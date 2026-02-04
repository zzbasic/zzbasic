# ZzBasic - Documentação v0.5.0

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

## Versão Atual: **0.5.0**

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

---
![zzbasic_repl](zzbasic_0_5_0.png)
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
| 0.5.1 | **em desenvolvimento** | operadores de comparação |

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
cd zzbasic/0.4.3

# Compile
make

# Execute
./zzbasic
```

### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.4.3
gcc -o zzbasic *.c
zzbasic.exe
```

---

## 📚 Referência da Linguagem

### Gramática EBNF (v0.5.0)

```
=====================================================================
ZzBasic - GRAMÁTICA v0.5.0 
Estruturas de Controle: if, else if, else
Última atualização: 20260402
=====================================================================

=====================================================================
PROGRAMA ESTRUTURAL 
=====================================================================
program             := statement_list

statement_list      := statement (EOL statement)* EOL?

statement       := assignment_stmt
                | print_stmt
                | color_stmt 
                | input_stmt 
                | if_stmt
                | expression_stmt


=====================================================================
assignment_stmt 
=====================================================================
assignment_stmt := 'let' IDENTIFIER '=' expression


=====================================================================
print_stmt     
=====================================================================
print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)

print_item      := expression 
                | format_directive
                | color_directive

format_directive := alignment
                  | width_spec

alignment       := 'left' | 'right' | 'center'

width_spec      := 'width' '(' NUMBER ')'

color_directive := color_name 
                | 'nocolor'

color_name      := normal_color
                | bright_color
                | background_color

normal_color    := 'black' | 'red' | 'green' | 'yellow'
                | 'blue' | 'magenta' | 'cyan' | 'white'

bright_color    := 'bred' | 'bgreen' | 'byellow' | 'bblue'
                | 'bmagenta' | 'bcyan' | 'bwhite'

background_color := 'bgblack' | 'bgred' | 'bggreen' | 'bgyellow'
                  | 'bgblue' | 'bgmagenta' | 'bgcyan' | 'bgwhite'



=====================================================================
color_stmt 
=====================================================================
color_stmt := COLOR_TOKEN


=====================================================================
input_stmt
=====================================================================
input_stmt      := 'input' (STRING)? IDENTIFIER  


=====================================================================
if_stmt
=====================================================================
if_stmt         := 'if' logical_expr 'then' EOL   
                       statement*
                   else_if_part*
                   else_part?
                   'end' 'if' EOL

else_if_part    := 'else' 'if' logical_expr 'then' EOL
                       statement*

else_part       := 'else' EOL
                       statement*


=====================================================================
expression_stmt 
=====================================================================
expression_stmt  := logical_expr

logical_expr     := logical_or_expr

logical_or_expr  := logical_and_expr ('or' logical_and_expr)*

logical_and_expr := comparison_expr ('and' comparison_expr)*

comparison_expr  := expression (comparison_op expression)*

comparison_op    := '==' | '!=' | '<' | '>' | '<=' | '>='


=====================================================================
expression 
=====================================================================
expression       := term (('+' | '-') term)*

term             := factor (('*' | '/') factor)*

factor           := ('+' | '-'| 'not' | '!')? atom

atom             := NUMBER | STRING | BOOLEAN | IDENTIFIER | '(' expression ')' 


=====================================================================
LITERAIS 
=====================================================================
COMMENT         := '#' ~[\n\r]*

IDENTIFIER      := [a-zA-Z_][a-zA-Z0-9_]*

NUMBER          := [0-9]+ ('.' [0-9]+)?

STRING          := '"' [^"]* '"'

BOOLEAN         := 'true' | 'false'


=====================================================================
PALAVRAS-CHAVE 
=====================================================================
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

black
red
green
yellow
blue
magenta
cyan
white

bred
bgreen
byellow
bblue
bmagenta
bcyan
bwhite

bgblack
bgred
bggreen
bgyellow
bgblue
bgmagenta
bgcyan
bgwhite


=====================================================================
OPERADORES 
=====================================================================
PLUS            := '+'
MINUS           := '-'
MULT            := '*'
DIV             := '/'
ASSIGN          := '='
EQUAL           := '='
NOT_EQUAL       := '!='
LESS            := '<'
GREATER         := '>'
LESS_EQUAL      := '<='
GREATER_EQUAL   := '>='
LPAREN          := '('
RPAREN          := ')'

AND             := 'and'
OR              := 'or' 
NOT             := 'not' | !'


=====================================================================
HIERARQUIA DE PRECEDÊNCIA (do menor para o MAIOR)
=====================================================================
Nível   Operador                Associatividade     Função do Parser
1       print/?, input, nl, if  -                   cada recurso tem a sua    
2       =                       -                   parse_assignment_stmt()
3       or                      Esquerda            parse_logical_or_expr()
4       and                     Esquerda            parse_logical_and_expr()
5       ==, !=, <, >, <=, >=    Esquerda            parse_comparison_expr()
6       +, -                    Esquerda            parse_expression()
7       *, /                    Esquerda            parse_term()
8       +, -, not, ! (unários)  Direita             parse_factor()
9       Átomos, parênteses      -                   parse_atom()


=====================================================================
NOTAS IMPORTANTES SOBRE O PRINT 
=====================================================================
# 1. 'nl' é OPCIONAL no final do print
# 2. Comportamento padrão: acumula na mesma linha
# 3. Com 'nl' no final: quebra linha após imprimir
# 4. Espaço é separador padrão entre print_items


=====================================================================
NOTAS SOBRE IF...ELSE
=====================================================================
# 1. IF sempre requer THEN
# 2. ELSE IF é opcional e pode ser encadeado
# 3. ELSE é opcional e deve ser o último bloco
# 4. IF sempre termina com END IF
# 5. Cada bloco (IF, ELSE IF, ELSE) contém zero ou mais statements


FIM DA GRAMÁTICA DO ZzBasic v0.5.0
=====================================================================

```

---

## 🚀 Próximas Versões

### v0.5.1 
- Operadores de comparação e lógicos ('=', '<>', '<', '>', '<=', '>=', AND, OR, NOT)

---

## 📖 Documentação Adicional

Para mais detalhes, consulte o **Manual do Usuário** (em desenvolvimento).

---

## 🤝 Contribuições

ZzBasic é um projeto solo, mas sugestões e feedback são bem-vindos!

---

## 📄 Licença

MIT License

Copyright (c) 2026 arataca89

---

**Última atualização:** Fevereiro de 2026
**Versão:** 0.5.0

