# ZzBasic - Documentação v0.4.2

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

## Versão Atual: **0.4.2** ✅

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
- ✅ **Largura de campo (`width()`) e alinhamento (`left`, `right`, `center`)** - Formatação de tabelas

### 📋 Histórico de Versões

| Versão | Status | Recursos Principais |
|--------|--------|-------------------|
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| 0.4.0 | Lançada | Comando `print`, controle de linha, saída formatada |
| 0.4.1 | Lançada | Comando `print` com cores (24 cores) |
| 0.4.2 | Lançada | Largura de campo e alinhamento no `print` |
| **0.4.3** | **Em desenvolvimento** | Comando `input` |

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
cd zzbasic/0.4.2

# Compile
make
# ou gcc -o zzbasic *.c -lm

# Execute
./zzbasic
```

### Compilação (Windows)
[tdm-gcc](https://jmeubank.github.io/tdm-gcc/)

```bash
# Com TDM-GCC
cd zzbasic\0.4.2
gcc -o zzbasic *.c 
zzbasic.exe
```

### Testando

```bash
# REPL
$ ./zzbasic
ZzBasic v0.4.2 on Linux
>> 5 + 3 * 2
11
>> exit

# Arquivo
$ echo 'let x = 10 : let y = 20 : print x + y nl' > teste.zz
$ ./zzbasic teste.zz
30
```

---

## 🎯 Tutorial Rápido v0.4.2

### 1. Expressões Aritméticas Básicas

```basic
>> 2 + 3
5

>> 10 - 4
6

>> 5 * 6
30

>> 20 / 4
5

>> (2 + 3) * 4
20

>> -5 + 10
5
```

### 2. Variáveis com `let`

```basic
>> let pi = 3.14159
>> let raio = 10
>> let area = pi * (raio * raio)
>> print "Área: " area nl
Área: 314.159

>> let nome = "ZzBasic"
>> let versao = "0.4.2"
>> print nome " " versao nl
ZzBasic 0.4.2
```

### 3. Comando `print` Básico

#### Imprimindo valores simples:

```basic
>> print "Olá Mundo" nl
Olá Mundo

>> print 42 nl
42

>> let nome = "Zurg"
>> print "Bem-vindo, " nome "!" nl
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
>> print "." 
>> print "." 
>> print "." 
>> print "]" nl
Progresso: [...]
```

### 4. 🎨 Cores no `print`

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

### 5. 📊 Tabela de Cores Disponíveis

| Categoria | Cores |
|-----------|-------|
| **Normais** | `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white` |
| **Brilhantes** | `bblack`, `bred`, `bgreen`, `byellow`, `bblue`, `bmagenta`, `bcyan`, `bwhite` |
| **Fundo** | `bgblack`, `bgred`, `bggreen`, `bgyellow`, `bgblue`, `bgmagenta`, `bgcyan`, `bgwhite` |
| **Especial** | `nocolor` (reseta a cor para padrão) |

### 6. 📐 NOVO: Formatação com `width()` e Alinhamento

A v0.4.2 introduz formatação profissional de campos com largura e alinhamento.

#### Largura de campo com `width()`:

```basic
>> print width(20) left "Nome" nl
Nome                

>> print width(20) right "Valor" nl
                Value

>> print width(20) center "Centro" nl
         Centro         
```

#### Criando tabelas:

```basic
>> print width(15) left "Produto" " | " width(10) right "Preço" nl
Produto         |      Preço
>> print width(15) left "Notebook" " | " width(10) right 4500.50 nl
Notebook        |    4500.50
>> print width(15) left "Mouse" " | " width(10) right 89.90 nl
Mouse           |      89.90
```

#### Combinando cores e formatação:

```basic
>> print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
           ALERTA DE SEGURANÇA
(com fundo vermelho, centralizado em 40 caracteres)

>> print width(30) left "Status" " | " width(15) center green "ATIVO" nocolor nl
Status                     |       ATIVO
```

#### Exemplo prático completo:

```basic
# Cabeçalho da tabela
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "──────────────────────────────────────────" nocolor nl

# Dados
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
```

### 7. 📖 Filosofia do `print` no ZzBasic

O ZzBasic adota uma abordagem **simplificada e consistente** para o comando `print`:

#### Princípios de Design

Um comando deve fazer uma coisa bem. No ZzBasic, o `print` é responsável por exibir valores com controle total sobre cores, formatação e nova linha. Se você precisa imprimir múltiplos valores com cores diferentes, faça em um único `print`. Se precisa controlar linhas, use `nl` explicitamente.

#### O que TEMOS

```basic
# Um print, múltiplos itens com cores (CORRETO)
print red "Erro:" nocolor " Arquivo não encontrado" nl

# Cores em diferentes partes
print "Status: " green "OK" nocolor " [" yellow "Aguardando" nocolor "]" nl

# Expressões matemáticas com cores
print "Resultado: " blue 10 + 20 nocolor " = 30" nl

# Formatação com width e alinhamento
print width(15) left "Nome" " | " width(20) right "Valor" nl
print width(15) left "João" " | " width(20) right 1500.00 nl

# Combinação de cores e formatação
print red width(30) center "ALERTA" nocolor nl
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

### 8. 🎮 REPL Avançado

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

# Testando formatação
>> tokens print width(20) left "Nome" nl
=== LEXICAL ANALYSIS ===
  1: (1:1)[PRINT]: print
  2: (1:7)[WIDTH]: width
  3: (1:12)[LPAREN]: (
  4: (1:13)[NUMBER]: 20
  5: (1:15)[RPAREN]: )
  6: (1:17)[LEFT]: left
  7: (1:22)[STRING]: "Nome"
  8: (1:28)[NL]: nl
  9: (1:30)[EOF]
=== END OF ANALYSIS ===
```

---

## 📚 Referência da Linguagem v0.4.2

### Gramática EBNF

```ebnf
# =======================================================
# ZzBasic - GRAMÁTICA v0.4.2
# =======================================================

# ---------- PROGRAMA ESTRUTURAL ----------
program         := (statement | comment)* EOF
statement       := assignment_stmt | print_stmt | expression_stmt

assignment_stmt := LET identifier '=' expression

expression_stmt := expression

expression      := term (('+' | '-') term)*
term            := factor (('*' | '/') factor)*
factor          := ('+' | '-')? atom
atom            := NUMBER | STRING | identifier | '(' expression ')' 

comment         := '#' ~[\n\r]*

identifier      := [a-zA-Z_][a-zA-Z0-9_]*


# ------------ PRINT STATEMENT ----------------------------
print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)

print_item      := expression 
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
             | background_color

normal_color ::= 'black' | 'red' | 'green' | 'yellow'
               | 'blue' | 'magenta' | 'cyan' | 'white'

bright_color ::= 'bred' | 'bgreen' | 'byellow' | 'bblue'
               | 'bmagenta' | 'bcyan' | 'bwhite'

background_color ::= 'bgblack' | 'bgred' | 'bggreen' | 'bgyellow'
                   | 'bgblue' | 'bgmagenta' | 'bgcyan' | 'bgwhite'

nl              := 'nl'        # New line - quando presente, quebra linha


# ---------- NOTAS IMPORTANTES ----------
# 1. 'nl' é OPCIONAL no final do print
# 2. Comportamento padrão: acumula na mesma linha
# 3. Com 'nl' no final: quebra linha após imprimir
# 4. Espaço é separador padrão entre print_items
# 5. width() aceita valores entre 1 e 256
# 6. Alinhamento padrão é LEFT se não especificado
```

### Exemplos Válidos v0.4.2

```basic
# Comentários
let x = 10
let y = x + 5
x + y           # Expressão sem LET
let z = (x + 3) * 2

# Comando PRINT básico
print "Olá Mundo" nl
print x nl
print y + z nl
print "Valor:" x "Resultado:" y * 2 nl

# Atalho com ?
? "Teste rápido" nl
? 10 + 20 nl

# Controle de nova linha
print "Linha 1"
print " continua"
print "Fim!" nl

# Cores
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print green "SUCESSO" nocolor " - Operação concluída" nl
print blue "Informação:" nocolor " Verifique os dados" nl

# Combinações de cores
print bred bgblue "CRÍTICO" nocolor " - Sistema em risco" nl
print yellow "AVISO" nocolor " - Ação necessária" nl

# Formatação (v0.4.2)
print width(20) left "Nome" " | " width(10) right "Valor" nl
print width(20) left "João" " | " width(10) right 1500.00 nl

# Cores + Formatação
print red width(30) center "ALERTA" nocolor nl
print green width(30) center "SUCESSO" nocolor nl

# Múltiplos statements
let a = 5 : print red a nocolor " ao quadrado:" a * a nl
```

---

## 🚀 Próximas Versões

### v0.4.3 
- Comando `input` para entrada de dados do usuário

### v0.5.0
- Estruturas de controle: `if`, `else`

### v0.6.0
- Loops: `for`, `while`

### v0.7.0
- Funções definidas pelo usuário
- Escopo de variáveis

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

**Última atualização:** Janeiro de 2026  
**Versão:** 0.4.2  
**Status:** ✅ Lançada
