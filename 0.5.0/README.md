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
# =======================================================
# ZzBasic - GRAMÁTICA v0.5.0 
# Estruturas de Controle: if, else if, else
# =======================================================

# ---------- PROGRAMA ESTRUTURAL ----------
program         := (statement | COMMENT)* EOF

statement       := assignment_stmt
                | input_stmt 
                | print_stmt
                | color_stmt 
                | if_stmt
                | expression_stmt

assignment_stmt := LET IDENTIFIER '=' expression

expression_stmt := expression

input_stmt      := 'INPUT' (STRING)? IDENTIFIER  

print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)

color_stmt      := COLOR_TOKEN 

# ---------- ESTRUTURAS DE CONTROLE ----------
if_stmt         := 'IF' logical_expr 'THEN' EOL   
                       statement*
                   else_if_part*
                   else_part?
                   'END' 'IF' EOL

else_if_part    := 'ELSE' 'IF' logical_expr 'THEN' EOL
                       statement*

else_part       := 'ELSE' EOL
                       statement*

# ---------- EXPRESSÕES LÓGICAS ----------
logical_expr     := logical_or_expr    

logical_or_expr  := logical_and_expr ('OR' logical_and_expr)*  

logical_and_expr := comparison_expr ('AND' comparison_expr)*  

comparison_expr  := expression (relational_op expression)* 

relational_op    := '=' | '<>' | '<' | '>' | '<=' | '>='

# ---------- EXPRESSÕES ARITMÉTICAS ----------
expression      := term (('+' | '-') term)*

term            := factor (('*' | '/') factor)*

factor          := ('+' | '-')? atom

atom            := NUMBER | STRING | BOOLEAN | IDENTIFIER | '(' expression ')' 

# ---------- PRINT ITEMS ----------
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

# ---------- TOKENS E LITERAIS ----------
COMMENT         := '#' ~[\n\r]*

IDENTIFIER      := [a-zA-Z_][a-zA-Z0-9_]*

NUMBER          := [0-9]+ ('.' [0-9]+)?

STRING          := '"' [^"]* '"'

BOOLEAN         := 'true' | 'false'

# ---------- PALAVRAS-CHAVE ----------
LET             := 'let'
PRINT           := 'print'
INPUT           := 'input'
IF              := 'if'
THEN            := 'then'
ELSE            := 'else'
END             := 'end'
AND             := 'and'
OR              := 'or'
NL              := 'nl'

# ---------- OPERADORES ----------
PLUS            := '+'
MINUS           := '-'
MULT            := '*'
DIV             := '/'
ASSIGN          := '='
EQUAL           := '='
NOT_EQUAL       := '<>'
LESS            := '<'
GREATER         := '>'
LESS_EQUAL      := '<='
GREATER_EQUAL   := '>='
LPAREN          := '('
RPAREN          := ')'

# ---------- NOTAS IMPORTANTES SOBRE O PRINT ----------
# 1. 'nl' é OPCIONAL no final do print
# 2. Comportamento padrão: acumula na mesma linha
# 3. Com 'nl' no final: quebra linha após imprimir
# 4. Espaço é separador padrão entre print_items

# ---------- NOTAS SOBRE IF/ELSE/ELSE IF ----------
# 1. IF sempre requer THEN
# 2. ELSE IF é opcional e pode ser encadeado
# 3. ELSE é opcional e deve ser o último bloco
# 4. IF sempre termina com END IF
# 5. Cada bloco (IF, ELSE IF, ELSE) contém zero ou mais statements

# =====================================================================
# HIERARQUIA DE PRECEDÊNCIA DE OPERADORES
# =====================================================================
# 1. ()                  - Parênteses
# 2. + - (unário)        - Positivo/Negativo
# 3. * /                 - Multiplicação/Divisão
# 4. + - (binário)       - Adição/Subtração
# 5. = <> < > <= >=      - Comparação (relational_op)
# 6. AND                 - Conjunção lógica
# 7. OR                  - Disjunção lógica
# 8. = (em let)          - Atribuição
# 9. print/?, nl, if     - Comandos e controle de fluxo

# =====================================================================
# EXEMPLOS DE USO
# =====================================================================

# Exemplo 1: IF simples
# if x > 10 then
#     print "x é maior que 10" nl
# end if

# Exemplo 2: IF com ELSE
# if x > 10 then
#     print "x é maior que 10" nl
# else
#     print "x é menor ou igual a 10" nl
# end if

# Exemplo 3: IF com ELSE IF
# if x > 10 then
#     print "x é maior que 10" nl
# else if x > 5 then
#     print "x é maior que 5" nl
# else if x > 0 then
#     print "x é positivo" nl
# else
#     print "x é zero ou negativo" nl
# end if

# Exemplo 4: Expressões lógicas
# if x > 5 and y < 10 then
#     print "x > 5 AND y < 10" nl
# end if

# if a = 1 or b = 2 then
#     print "a = 1 OR b = 2" nl
# end if

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

