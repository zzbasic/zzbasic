# ZzBasic - Documentação v0.4.3

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

## Versão Atual: **0.4.3** 🚧

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

### 📋 Histórico de Versões

| Versão | Status | Recursos Principais |
|--------|--------|-------------------|
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| 0.4.0 | Lançada | Comando `print`, controle de linha, saída formatada |
| 0.4.1 | Lançada | Comando `print` com cores |
| 0.4.2 | Lançada | Largura de campo e alinhamento no `print` |
| **0.4.3** | **Lançada** | `input` |

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

### Testando

```bash
# REPL
$ ./zzbasic
ZzBasic v0.4.3 on Linux
>> input "Qual o seu nome? " nome
Qual o seu nome? Zurg
>> print "Olá, " nome nl
Olá, Zurg
>> exit

# Arquivo
$ echo 'input "Digite um número: " n : print "O dobro é: " n * 2 nl' > teste.zz
$ ./zzbasic teste.zz
Digite um número: 10
O dobro é: 20
```

---

## 🎯 Tutorial Rápido v0.4.3

### 1. Comando `input`

O comando `input` permite que você peça dados ao usuário e os armazene em uma variável.

#### Sintaxe

```basic
input [prompt] <variável>
```

- **`prompt` (opcional):** Uma string que será exibida para o usuário.
- **`<variável>`:** O nome da variável onde o dado será armazenado.

#### Exemplos

**Sem prompt:**

```basic
>> input nome
>> print "Olá, " nome nl
```

**Com prompt:**

```basic
>> input "Qual a sua idade? " idade
>> print "Você tem " idade " anos." nl
```

### 2. Detecção Automática de Tipo

O ZzBasic detecta automaticamente se o usuário digitou um número ou uma string:

```basic
>> input "Digite um número: " n
Digite um número: 10
>> print n * 2 nl
20

>> input "Digite seu nome: " nome
Digite seu nome: Zurg
>> print "Olá, " nome nl
Olá, Zurg
```

### 3. Outros Recursos

(Seções sobre `print`, cores, formatação, etc. permanecem as mesmas da v0.4.2)

---

## 📚 Referência da Linguagem

### Gramática EBNF (v0.4.3)

```
program         := (statement | COMMENT)* EOF

statement       := assignment_stmt
                 | expression_stmt
                 | input_stmt 
                 | print_stmt

assignment_stmt := LET identifier '=' expression

expression_stmt := expression

input_stmt      := 'INPUT' (STRING)? IDENTIFIER

print_stmt      := ('print' | '?') print_item* ('nl' | EOL | EOF)

expression      := term (('+' | '-') term)*

term            := factor (('*' | '/') factor)*

factor          := ('+' | '-')? atom

atom            := NUMBER | STRING | IDENTIFIER | '(' expression ')' 

COMMENT         := '#' ~[\n\r]*

IDENTIFIER      := [a-zA-Z_][a-zA-Z0-9_]*

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

normal_color ::= 'black' | 'red' | 'green' | 'yellow'
               | 'blue' | 'magenta' | 'cyan' | 'white'

bright_color ::= 'bred' | 'bgreen' | 'byellow' | 'bblue'
               | 'bmagenta' | 'bcyan' | 'bwhite'
```


### Exemplos 

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

# input 
input "Entre com o valor do produto: " valor
input "Entre com o desconto: " desc
let desconto = valor * desc
let valor_a_pagar = valor - desconto
print "Valor do produto: " valor nl 
print "Desconto        : " desc  nl 
print "Valor a pagar   : " valor_a_pagar nl 

```

---

## 🚀 Próximas Versões

### v0.5.0 
- Estrutura condicional `if...then...else`

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
**Versão:** 0.4.3

