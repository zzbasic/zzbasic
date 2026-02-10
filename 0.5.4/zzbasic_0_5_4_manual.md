# Manual do Usuário - ZzBasic v0.5.4

Bem-vindo ao ZzBasic! Este manual é o seu guia completo para aprender e dominar a linguagem de programação ZzBasic. Aqui você encontrará tudo o que precisa para começar, desde a instalação até a criação de programas úteis.

## Capítulo 1: Introdução

### O que é ZzBasic?

ZzBasic é uma linguagem de programação interpretada, de tipagem dinâmica, que combina a simplicidade do BASIC clássico com recursos modernos. Foi projetada para ser fácil de aprender, mas poderosa o suficiente para criar aplicações úteis.

### Filosofia da Linguagem

O ZzBasic segue três princípios fundamentais:

- **Simplicidade**: A sintaxe é limpa e intuitiva, com comandos em caixa baixa e uma gramática consistente.
- **Clareza**: O código deve ser legível e fácil de entender. O ZzBasic favorece a clareza sobre a concisão.
- **Controle Explícito**: O programador tem controle total sobre o comportamento do programa, como quebras de linha e formatação.

### Instalação e Compilação

#### Pré-requisitos

- Compilador C: GCC, Clang ou MSVC
- Make (opcional) ou a89make
- Git (para clonar)

#### Compilação (Linux/macOS)

```bash
# Clone o repositório
git clone https://github.com/zzbasic/zzbasic.git
cd zzbasic/0.5.4

# Compile
a89make

# Execute
./zzbasic
```

#### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.5.4
gcc -o zzbasic *.c
zzbasic.exe
```

---

## Capítulo 2: Conceitos Básicos

### REPL vs. Scripts

O ZzBasic pode ser usado de duas maneiras:

- **REPL (Read-Eval-Print Loop)**: Um ambiente interativo para testar ideias rapidamente. Execute `zzbasic` sem argumentos para entrar no REPL.
- **Scripts**: Arquivos de texto com extensão `.zz` que contêm uma sequência de comandos. Execute `zzbasic <nome_do_arquivo>.zz` para rodar um script.

### Comentários

Comentários começam com `#` e vão até o final da linha. Eles são ignorados pelo interpretador e servem para documentar seu código.

```zzbasic
# Este é um comentário
let x = 10  # Este também é um comentário
```

Use comentários para explicar o que seu código faz. Isso ajuda você e outras pessoas a entender o programa depois.

### Tipos de Dados

O ZzBasic suporta três tipos de dados principais:

- **Números**: Números de ponto flutuante de dupla precisão (ex: `10`, `3.14`, `-5`).
- **Strings**: Sequências de caracteres entre aspas duplas (ex: `"Olá Mundo"`). O tamanho máximo de uma string ZzBasic é 255 caracteres.
- **Booleanos**: Valores verdadeiro ou falso (`true` ou `false`).

Você não precisa declarar o tipo de uma variável. O ZzBasic descobre automaticamente qual tipo é.

### Palavras-chave da Linguagem

O ZzBasic possui as seguintes palavras-chave reservadas (devem ser usadas em caixa baixa):

| Categoria | Palavras-chave |
|-----------|----------------|
| **Atribuição** | `let` |
| **Saída** | `print`, `?` |
| **Entrada** | `input` |
| **Controle de Fluxo** | `if`, `then`, `else`, `end if` |
| **Loops** | `while`, `do`, `end while`, `for`, `to`, `step`, `end for` |
| **Controle de Loop** | `break`, `continue` |
| **Operadores Lógicos** | `and`, `or`, `not` |
| **Booleanos** | `true`, `false` |
| **Formatação** | `nl`, `left`, `right`, `center`, `width` |
| **Cores** | `nocolor`, `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white` |
| **Cores Brilhantes** | `bblack`, `bred`, `bgreen`, `byellow`, `bblue`, `bmagenta`, `bcyan`, `bwhite` |
| **Cores de Fundo** | `bgblack`, `bgred`, `bggreen`, `bgyellow`, `bgblue`, `bgmagenta`, `bgcyan`, `bgwhite` |

---

## Capítulo 3: Variáveis e Expressões

### Criando Variáveis com `let`

O comando `let` é usado para criar e atribuir valores a variáveis. Uma variável é um espaço na memória que armazena um valor.

```zzbasic
let nome = "Zurg"
let idade = 120
let pi = 3.14
let ativo = true
```

Os nomes de variáveis devem começar com uma letra ou `_` e podem conter letras, números e `_`. Use nomes descritivos para tornar seu código mais legível.

```zzbasic
let nome_completo = "João Silva"  # Bom
let n = "João Silva"              # Ruim - nome não descritivo
```

### Expressões Aritméticas

O ZzBasic suporta as quatro operações aritméticas básicas:

- `+` (adição)
- `-` (subtração)
- `*` (multiplicação)
- `/` (divisão)

```zzbasic
let soma = 10 + 5
let produto = 3 * 4
let media = (8.5 + 7.5) / 2
```

### Operadores de Comparação

O ZzBasic suporta operadores de comparação para comparar valores:

- `==` (igual)
- `!=` (diferente)
- `<` (menor que)
- `>` (maior que)
- `<=` (menor ou igual)
- `>=` (maior ou igual)

```zzbasic
let x = 10
let y = 5

print x == y nl  # false
print x != y nl  # true
print x > y nl   # true
print x <= y nl  # false
```

### Operadores Lógicos

O ZzBasic suporta operadores lógicos para combinar condições:

- `and` (E lógico)
- `or` (OU lógico)
- `not` ou `!` (NÃO lógico)

```zzbasic
let x = 10
let y = 5

print (x > 5) and (y > 0) nl  # true
print (x > 20) or (y > 0) nl  # true
print not (x > 20) nl         # true
print ! (x > 20) nl           # true (! é atalho para not)
```

### Precedência de Operadores

O ZzBasic segue a ordem padrão de precedência de operadores:

1. Parênteses `()`
2. Operadores unários: `+`, `-`, `not`, `!`
3. Multiplicação e divisão: `*`, `/`
4. Adição e subtração: `+`, `-`
5. Comparação: `==`, `!=`, `<`, `>`, `<=`, `>=`
6. AND lógico: `and`
7. OR lógico: `or`

```zzbasic
let resultado = 2 + 3 * 4  # Resultado é 14, não 20
let resultado = (2 + 3) * 4  # Resultado é 20
let resultado = 5 > 3 and 2 < 4  # true
```

Use parênteses para deixar claro qual operação deve ser feita primeiro.

---

## Capítulo 4: Comando `print`

O comando `print` é usado para exibir valores na tela. Ele tem uma sintaxe flexível e poderosa.

### Imprimindo Valores Simples

```zzbasic
print "Olá Mundo"
print 42
print 3.14
print true
```

### Imprimindo Variáveis

```zzbasic
let nome = "Zurg"
print nome
```

### Imprimindo Múltiplos Valores

Você pode imprimir vários valores em um único comando. Eles são separados por espaços automaticamente.

```zzbasic
let nome = "Zurg"
let idade = 120
print "Nome:" nome
print "Idade:" idade
print "Resultado:" 10 + 5
```

Note que para imprimir vários valores no mesmo `print` estes devem estar separados por um espaço em branco.

### Imprimindo Expressões

Você pode imprimir o resultado de expressões diretamente:

```zzbasic
print 5 > 3 nl           # true
print true and false nl  # false
print 10 + 5 * 2 nl      # 20
```

### Controle de Nova Linha (`nl`)

Por padrão, o `print` não quebra a linha. Para quebrar a linha, use `nl` no final do comando.

```zzbasic
print "Olá"  # Fica na mesma linha
print "Mundo" nl # Quebra a linha
```

Isso é útil quando você quer construir uma linha gradualmente:

```zzbasic
print "Progresso: ["
print "."
print "."
print "."
print "]" nl
```

### Atalho `?`

O `?` é um atalho para `print`. Use quando quiser digitar menos:

```zzbasic
? "Teste rápido" nl
? 10 + 20 nl
? true nl
```

### Cores e Formatação

O `print` suporta 24 cores para tornar a saída mais expressiva e legível.

#### Cores Normais

```zzbasic
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print green "SUCESSO" nocolor " - Operação concluída" nl
print yellow "AVISO" nocolor ": Verifique os dados" nl
print blue "Informação" nocolor ": Leia com atenção" nl
```

#### Cores Brilhantes

```zzbasic
print bred "CRÍTICO" nocolor " - Sistema em risco" nl
print bgreen "OK" nocolor " - Tudo funcionando" nl
```

#### Cores de Fundo

```zzbasic
print bgred "ALERTA" nocolor " - Ação necessária" nl
print bgblue white "INFORMAÇÃO" nocolor " - Leia com atenção" nl
```

#### Combinações

```zzbasic
print bred bgblue "CRÍTICO" nocolor " - Sistema em risco" nl
print cyan "Dica:" nocolor " Use 'nocolor' para resetar" nl
```

#### Tabela de Cores

| Categoria | Cores |
|-----------|-------|
| **Normais** | `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white` |
| **Brilhantes** | `bblack`, `bred`, `bgreen`, `byellow`, `bblue`, `bmagenta`, `bcyan`, `bwhite` |
| **Fundo** | `bgblack`, `bgred`, `bggreen`, `bgyellow`, `bgblue`, `bgmagenta`, `bgcyan`, `bgwhite` |
| **Especial** | `nocolor` (reseta a cor para padrão) |

### Largura de Campo e Alinhamento

O `print` suporta controle de largura de campo e alinhamento para criar tabelas e formatação profissional.

#### Largura de Campo (`width`)

Use `width(n)` para especificar a largura mínima de um campo:

```zzbasic
print width(20) "Nome" nl
print width(10) 42 nl
```

#### Alinhamento

Use `left`, `right` ou `center` para controlar o alinhamento dentro do campo:

```zzbasic
print width(20) left "Esquerda" nl
print width(20) right "Direita" nl
print width(20) center "Centro" nl
```

#### Exemplos Práticos

**Tabela Simples:**

```zzbasic
print width(20) left "Produto" " | " width(10) right "Preço" nl
print width(20) left "Notebook" " | " width(10) right 4500.50 nl
print width(20) left "Mouse" " | " width(10) right 89.90 nl
```

**Combinando com Cores:**

```zzbasic
print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
print yellow width(40) center "Verifique as permissões" nocolor nl
```

**Tabela Formatada:**

```zzbasic
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "============================================" nocolor nl
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
```

---

## Capítulo 5: Entrada de Dados

### Comando `input`

O comando `input` é usado para ler uma linha de texto do usuário. A sintaxe é:

```zzbasic
input "mensagem" variavel
```

#### Exemplos Básicos

```zzbasic
input "Digite seu nome: " nome
print "Olá, " nome nl

input "Digite um número: " numero
print "Você digitou: " numero nl

input "Digite sua idade: " idade
if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

### `input` com Formatação (v0.5.4)

A partir da versão 0.5.4, o comando `input` suporta cores, largura de campo e alinhamento, assim como o comando `print`.

#### Cores no `input`

```zzbasic
input cyan "Digite seu nome: " nocolor nome
input red width(30) center "ENTRADA IMPORTANTE: " nocolor valor
```

#### Largura de Campo

```zzbasic
input width(40) "Digite um texto: " texto
input width(20) center "Valor: " numero
```

#### Alinhamento

```zzbasic
input width(30) left "Nome: " nome
input width(30) right "Idade: " idade
input width(30) center "Email: " email
```

#### Combinações

```zzbasic
input cyan width(40) center "Digite seu nome: " nocolor nome
input green width(30) left "Valor: " nocolor valor
```

---

## Capítulo 6: Controle de Fluxo

### Estrutura `if...else`

A estrutura `if...else` permite que você execute diferentes blocos de código baseado em condições.

#### Sintaxe Básica

```zzbasic
if (condicao) then
    # Código executado se a condição for verdadeira
else
    # Código executado se a condição for falsa
end if
```

#### Exemplo

```zzbasic
let idade = 20

if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

#### `else if` (Múltiplas Condições)

```zzbasic
let nota = 7.5

if (nota >= 9) then
    print "Excelente" nl
else if (nota >= 7) then
    print "Bom" nl
else if (nota >= 5) then
    print "Satisfatório" nl
else
    print "Insuficiente" nl
end if
```

### Loop `while`

O loop `while` executa um bloco de código enquanto uma condição for verdadeira.

#### Sintaxe

```zzbasic
while (condicao) do
    # Código executado enquanto a condição for verdadeira
end while
```

#### Exemplo

```zzbasic
let contador = 1

while (contador <= 5) do
    print contador nl
    let contador = contador + 1
end while
```

#### Saindo do Loop com `break`

```zzbasic
let numero = 0

while (numero < 100) do
    input "Digite um número (0 para sair): " numero
    if (numero == 0) then
        break
    end if
    print "Você digitou: " numero nl
end while
```

#### Pulando para a Próxima Iteração com `continue`

```zzbasic
let i = 0

while (i < 10) do
    let i = i + 1
    if (i == 5) then
        continue
    end if
    print i nl
end while
```

### Loop `for` (v0.5.4)

O loop `for` é uma maneira mais concisa de criar loops com um contador.

#### Sintaxe Básica

```zzbasic
for variavel = valor_inicial to valor_final do
    # Código executado para cada iteração
end for
```

#### Exemplo Simples

```zzbasic
for i = 1 to 10 do
    print i nl
end for
```

#### Com `step` (Incremento Customizado)

```zzbasic
for i = 1 to 30 step 2 do
    print i nl
end for
```

#### Com Expressões

```zzbasic
for i = 1 to 10 do
    let quadrado = i * i
    print i "² = " quadrado nl
end for
```

#### Usando `break` e `continue`

```zzbasic
for i = 1 to 20 do
    if (i == 5) then
        continue
    end if
    if (i == 15) then
        break
    end if
    print i nl
end for
```

---

## Capítulo 7: REPL Avançado (v0.5.4)

O REPL (Read-Eval-Print Loop) agora suporta modo multi-linha para escrever e debugar blocos de código.

### Comandos do REPL

#### Modo Normal

| Comando | Descrição |
|---------|-----------|
| `exit` ou `quit` | Sair do REPL |
| `help` ou `?` | Mostrar ajuda |
| `vars` | Listar variáveis |
| `reset` | Limpar todas as variáveis |
| `clear` | Limpar tela |
| `tokens "código"` | Mostrar tokens do código |
| `ast "código"` | Mostrar AST do código |
| `symbols` | Mostrar tabela de símbolos |

#### Modo Programa Multi-linha

| Comando | Descrição |
|---------|-----------|
| `program` | Entrar em modo programa |
| `end program` | Sair do modo programa e compilar |
| `run` | Executar programa em memória |
| `purge` | Limpar programa e variáveis da memória |
| `list` | Listar todas as linhas do programa |
| `list n` | Listar linha n |
| `list n-m` | Listar linhas de n até m |
| `list n-` | Listar linhas de n até o fim |
| `list -m` | Listar linhas do início até m |
| `edit n` | Editar linha n |
| `delete n` | Deletar linha n |
| `delete n-m` | Deletar linhas de n até m |
| `delete n-` | Deletar linhas de n até o fim |
| `delete -m` | Deletar linhas do início até m |
| `tokens program` | Mostrar tokens do programa |
| `ast program` | Mostrar AST do programa |
| `symbols program` | Mostrar símbolos do programa |
| `vars program` | Mostrar variáveis do programa |

### Exemplo de Uso do Modo Programa

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
>> list
01: for i = 1 to 10 do
02:     print i nl
03: end for
>> edit 2
02:     print i nl
02:     print "Número: " i nl
>> run
Número: 1
Número: 2
Número: 3
Número: 4
Número: 5
Número: 6
Número: 7
Número: 8
Número: 9
Número: 10
>> purge
[Memory purged]
>>
```

---

## Capítulo 8: Exemplo Completo - Jogo de Adivinhação

Aqui está um programa completo que demonstra vários conceitos do ZzBasic:

```zzbasic
# Jogo de Adivinhação
# Tente adivinhar o número secreto!

let secret_number = 42
let attempts = 0
let max_attempts = 5
let guessed_correctly = 0

print cyan "=========================================" nocolor nl
print cyan "       BEM-VINDO AO JOGO DE ADIVINHAÇÃO" nocolor nl
print cyan "=========================================" nocolor nl
print "" nl

print "Você tem " max_attempts " tentativas para adivinhar o número secreto." nl
print "O número está entre 1 e 100." nl
print "" nl

while (attempts < max_attempts) do
    let attempts = attempts + 1
    let attempts_left = max_attempts - attempts
    
    input yellow "Tentativa " attempts ": Digite um número: " nocolor guess
    
    if (guess == secret_number) then
        let guessed_correctly = 1
        break
    else if (guess > secret_number) then
        print blue "-> Muito alto!" nocolor nl
    else
        print blue "-> Muito baixo!" nocolor nl
    end if
    
    if (attempts_left > 0) then
        print yellow "Você ainda tem " attempts_left " tentativas." nocolor nl
    end if
end while

print "" nl
print cyan "=========================================" nocolor nl

if (guessed_correctly == 1) then
    print green "*** PARABÉNS! VOCÊ ACERTOU! ***" nocolor nl
    print "O número secreto era " secret_number "." nl
else
    print red "--- FIM DE JOGO! VOCÊ PERDEU! ---" nocolor nl
    print "O número secreto era " secret_number "." nl
end if

print cyan "=========================================" nocolor nl
print "Obrigado por jogar!" nl
```

---

## Capítulo 9: Próximos Passos (v0.6.0)

1. Funções e escopo
2. Arrays
3. Sistema de Módulos
4. Módulo math
5. Módulo string
6. Tipo text
7. Módulo io

---

## 📜 Licença

Este projeto é licenciado sob a Licença MIT. Veja o arquivo LICENSE para mais detalhes.

---

**Versão**: 0.5.4  
**Última atualização**: Fevereiro de 2026  
**Autor**: arataca89@gmail.com
