# ZzBasic - Documentação v0.4.1

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

## Versão Atual: **0.4.1 (EM DESENVOLVIMENTO)** 🚧

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

### 📋 Histórico de Versões

| Versão | Status | Recursos Principais |
|--------|--------|-------------------|
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| 0.4.0 | Lançada | Comando `print`, controle de linha, saída formatada |
| 0.4.1 | Lançada | Comando `print` com cores |
| **0.4.2** | **Em desenvolvimento** | **largura de campo e alinhamento** no `print` |

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
cd zzbasic/0.4.1

# Compile
make

# Execute
./zzbasic
```

### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.4.1
gcc -o zzbasic *.c
zzbasic.exe
```

### Testando

```bash
# REPL
$ ./zzbasic
ZzBasic v0.4.1 on Linux
>> 5 + 3 * 2
11
>> exit

# Arquivo
$ echo 'let x = 10 : let y = 20 : print x + y nl' > teste.zz
$ ./zzbasic teste.zz
30
```

---

## 🎯 Tutorial Rápido v0.4.1

### 1. Comando `print` Básico

#### Imprimindo valores simples:

```basic
>> print "Olá Mundo" nl
Olá Mundo

>> print 42 nl
42

>> let nome = "Zurg"
>> print "Bem-vindo," nome "!" nl
Bem-vindo, Zurg!
```

#### Atalho `?`:

```basic
>> ? "Teste rápido" nl
Teste rápido

>> ? 10 + 20 nl
30
```

#### Sem `nl` (acumula na linha):

```basic
>> print "Progresso: [" 
>> print "." print "." print "." 
>> print "]" nl
Progresso: [...]
```

### 2. 🎨 NOVO: Cores no `print`

O ZzBasic suporta **24 cores** para tornar a saída mais expressiva e legível.

#### Cores básicas:

```basic
>> print red "ERRO" nocolor ": Arquivo não encontrado" nl
ERRO: Arquivo não encontrado
(com "ERRO" em vermelho)

>> print green "SUCESSO" nocolor " - Operação concluída" nl
SUCESSO - Operação concluída
(com "SUCESSO" em verde)

>> print yellow "AVISO" nocolor ": Verifique os dados" nl
AVISO: Verifique os dados
(com "AVISO" em amarelo)
```

#### Cores brilhantes:

```basic
>> print bred "CRÍTICO" nocolor " - Reinicie o sistema" nl
CRÍTICO - Reinicie o sistema
(com "CRÍTICO" em vermelho brilhante)

>> print bgreen "OK" nocolor " - Tudo funcionando" nl
OK - Tudo funcionando
(com "OK" em verde brilhante)
```

#### Cores de fundo:

```basic
>> print bgred "ALERTA" nocolor " - Ação necessária" nl
ALERTA - Ação necessária
(com fundo vermelho)

>> print bgblue white "INFORMAÇÃO" nocolor " - Leia com atenção" nl
INFORMAÇÃO - Leia com atenção
(com fundo azul e texto branco)
```

#### Combinações:

```basic
>> print bred bgblue "CRÍTICO" nocolor " - Sistema em risco" nl
CRÍTICO - Sistema em risco
(vermelho brilhante em fundo azul)

>> print cyan "Dica:" nocolor " Use 'nocolor' para resetar" nl
Dica: Use 'nocolor' para resetar
(com "Dica:" em ciano)
```

### 3. 📊 Tabela de Cores Disponíveis

| Categoria | Cores |
|-----------|-------|
| **Normais** | `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white` |
| **Brilhantes** | `bblack`, `bred`, `bgreen`, `byellow`, `bblue`, `bmagenta`, `bcyan`, `bwhite` |
| **Fundo** | `bgblack`, `bgred`, `bggreen`, `bgyellow`, `bgblue`, `bgmagenta`, `bgcyan`, `bgwhite` |
| **Especial** | `nocolor` (reseta a cor para padrão) |

### 4. 📖 Filosofia do `print` no ZzBasic

O ZzBasic adota uma abordagem **simplificada e consistente** para o comando `print`:

#### Princípios de Design

Um comando deve fazer uma coisa bem. No ZzBasic, o `print` é responsável por exibir valores com controle total sobre cores e nova linha. Se você precisa imprimir múltiplos valores com cores diferentes, faça em um único `print`. Se precisa controlar linhas, use `nl` explicitamente.

#### O que TEMOS

```basic
# Um print, múltiplos itens com cores (CORRETO)
print red "Erro:" nocolor " Arquivo não encontrado" nl

# Cores em diferentes partes
print "Status: " green "OK" nocolor " [" yellow "Aguardando" nocolor "]" nl

# Expressões matemáticas com cores
print "Resultado: " blue 10 + 20 nocolor " = 30" nl
```

#### O que NÃO TEMOS (e por quê)

```basic
# ❌ NÃO FAÇA: Dois prints sem separador
print "A" print "B"      # ERRO: print não é expressão válida

# ❌ NÃO FAÇA: ; após print  
print "A"; print "B"     # ERRO: print não precisa de ;

# ❌ NÃO FAÇA: : após print
print "A": print "B"     # ERRO: use nl (nova linha)
```

### 5. 🎮 REPL Avançado

```basic
# Testando cores no REPL
>> tokens print red "Erro" nocolor nl
=== LEXICAL ANALYSIS ===
  1: (1:1)[PRINT]: print
  2: (1:7)[RED]: red
  3: (1:11)[STRING]: "Erro"
  4: (1:17)[NOCOLOR]: nocolor
  5: (1:25)[NL]: nl
  6: (1:27)[EOF]
=== END OF ANALYSIS ===

>> ast print red "Erro" nocolor nl
AST for: "print red "Erro" nocolor nl"
[1:1] STATEMENT_LIST (1 statements)
    [1:1] PRINT (3 items) [newline]
        [1:7] COLOR: red (\033[31m)
        [1:11] STRING: "Erro"
        [1:17] COLOR: nocolor (\033[0m)
```

---

## 📚 Referência da Linguagem v0.4.1

### Gramática EBNF

```ebnf
# =================================================
# ZzBasic - GRAMÁTICA v0.4.1 - PRINT com cores
# =================================================

# ---------- PROGRAMA ESTRUTURAL ----------
program         := (statement | comment)* EOF
statement       := assignment_stmt | print_stmt | expression_stmt | color_stmt

assignment_stmt := LET identifier '=' expression

expression_stmt := expression

color_stmt := COLOR_TOKEN (apenas nocolor por enquanto)

expression      := term (('+' | '-') term)*
term            := factor (('*' | '/') factor)*
factor          := ('+' | '-')? atom
atom            := NUMBER | STRING | identifier | '(' expression ')' 

comment         := '#' ~[\n\r]*

identifier      := [a-zA-Z_][a-zA-Z0-9_]*


# ------------ PRINT STATEMENT ----------------------------
print_stmt      := ('print' | '?') print_item* (nl | EOL | EOF)
print_item      := expression  
nl              := 'nl'        # New line - quando presente, quebra linha

print_item ::= expression 
             | format_directive
             | color_directive

format_directive ::= alignment
                   | width_spec

alignment ::= 'left' | 'right' | 'center'

width_spec ::= 'width' '(' number ')'

color_directive ::= color_name 
                  | 'nocolor'

color_name ::= normal_color
             | bright_color
             | background_color  # Futuro

normal_color ::= 'black' | 'red' | 'green' | 'yellow'
               | 'blue' | 'magenta' | 'cyan' | 'white'

bright_color ::= 'bred' | 'bgreen' | 'byellow' | 'bblue'
               | 'bmagenta' | 'bcyan' | 'bwhite'

background_color ::= 'bgblack' | 'bgred' | 'bggreen' | 'bgyellow'
                   | 'bgblue' | 'bgmagenta' | 'bgcyan' | 'bgwhite'


# ---------- NOTAS IMPORTANTES ----------
# 1. 'nl' é OPICIONAL no final do print
# 2. Comportamento padrão: acumula na mesma linha
# 3. Com 'nl' no final: quebra linha após imprimir
# 4. Espaço é separador padrão entre print_items
#--------------------- PRINT STATMENT FIM ----------------------------------



# ---------- LITERAIS ----------
NUMBER          := [0-9]+ ('.' [0-9]+)?
STRING          := '"' [^"]* '"'

# ---------- PALAVRAS-CHAVE ----------
LET             := 'let'
PRINT           := 'print'
?               := '?' (atalho para print)
NL              := 'nl'        # controle de linha no print


# ---------- FORMATAÇÃO (v0.4.1) ----------
# WIDTH          := 'width' '(' NUMBER ')'
# LEFT           := 'left'
# RIGHT          := 'right'
# CENTER         := 'center'
# NOCOLOR        := 'nocolor'

# ---------- CORES (v0.4.1) ----------
# BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
# BRED, BGREEN, BYELLOW, BBLUE, BMAGENTA, BCYAN, BWHITE
# BGBLACK, BGRED, BGGREEN, BGYELLOW, BGBLUE, BGMAGENTA, BGCYAN, BGWHITE

=====================================================================
Exemplos válidos v0.4.0:
=====================================================================

// Comentário
LET x = 10
LET y = x + 5
x + y           // Expressão sem LET
LET z = (x + 3) * 2

// NOVO: Comando PRINT
print "Olá Mundo"
print x
print y + z
print "Valor:" x "Resultado:" y * 2

// Atalho com ?
? "Teste rápido"
? 10 + 20

// Controle de nova linha
print "Linha 1";
print " continua"
print "Fim!"

// Múltiplos statements
let a = 5 : print a ; print " ao quadrado:" a * a

=====================================================================
Exemplos futuros v0.4.1 (formatação e cores):
=====================================================================

print red "ERRO" nocolor ": Arquivo não encontrado"
print green "SUCESSO!" nocolor width(20) "OK"

print left width(10) "ID" right width(20) "NOME"
print "101" width(10) "João" width(20) cyan "Ativo" nocolor

print bred bgblue "ALERTA" nocolor " Mensagem importante"

# ---------- NOTAS DE IMPLEMENTAÇÃO ----------
1. ';' no final do print suprime nova linha
2. Espaço é separador padrão entre print_items
3. '?' é equivalente a 'print' (atalho)
4. print vazio (sem argumentos) → linha em branco
5. Cores e formatação serão adicionadas na v0.4.1

=====================================================================
Hierarquia de precedência (mantida):
=====================================================================
1. ()                  - Parênteses
2. + - (unário)        - Positivo/Negativo
3. * /                 - Multiplicação/Divisão
4. + - (binário)       - Adição/Subtração
5. = (em LET)          - Atribuição
6. print/?, ;          - Comando print e controle de linha

**A gramática agora reflete o design completo do `print`!** 🎯

Vamos implementar o parser seguindo esta gramática?
```

### Exemplos Válidos v0.4.1

```basic
# Comentários
let x = 10
let y = x + 5
x + y           # Expressão sem LET
let z = (x + 3) * 2

# Comando PRINT básico
print "Olá Mundo"
print x
print y + z
print "Valor:" x "Resultado:" y * 2

# Atalho com ?
? "Teste rápido"
? 10 + 20

# Controle de nova linha
print "Linha 1"
print " continua"
print "Fim!"

# NOVO: Cores
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print green "SUCESSO" nocolor " - Operação concluída" nl
print blue "Informação:" nocolor " Verifique os dados" nl

# Combinações de cores
print bred bgblue "CRÍTICO" nocolor " - Sistema em risco" nl
print yellow "AVISO" nocolor " - Ação necessária" nl

# Múltiplos statements
let a = 5 : print red a nocolor " ao quadrado:" a * a nl
```

---

## 🚀 Próximas Versões

### v0.4.2 
- Largura de campo e alinhamento no `print`

---

## 📖 Documentação Adicional

Para mais detalhes, consulte o **Manual do Usuário** (em desenvolvimento).

---

## 🤝 Contribuições

ZzBasic é um projeto solo, mas sugestões e feedback são bem-vindos!

---

## 📄 Licença

[Especificar licença aqui]

---

**Última atualização:** Janeiro de 2026
**Versão:** 0.4.1
