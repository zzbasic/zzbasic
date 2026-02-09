# Manual do Usuário - ZzBasic v0.5.3

![ZzBasic](zzbasic_logo1.png)

Bem-vindo ao ZzBasic! Este manual é o seu guia completo para aprender e dominar a linguagem de programação ZzBasic. Aqui você encontrará tudo o que precisa para começar, desde a instalação até a criação de programas úteis.

---

## Capítulo 1: Introdução

### O que é ZzBasic?

ZzBasic é uma linguagem de programação interpretada, de tipagem dinâmica, que combina a simplicidade do BASIC clássico com recursos modernos. Foi projetada para ser fácil de aprender, mas poderosa o suficiente para criar aplicações úteis.

### Filosofia da Linguagem

O ZzBasic segue três princípios fundamentais:

1. **Simplicidade:** A sintaxe é limpa e intuitiva, com comandos em caixa baixa e uma gramática consistente.
2. **Clareza:** O código deve ser legível e fácil de entender. O ZzBasic favorece a clareza sobre a concisão.
3. **Controle Explícito:** O programador tem controle total sobre o comportamento do programa, como quebras de linha e formatação.

### Instalação e Compilação

#### Pré-requisitos

- **Compilador C**: GCC, Clang ou MSVC
- **Make** (opcional)
- **Git** (para clonar)

#### Compilação (Linux/macOS)

```bash
# Clone o repositório
git clone https://github.com/zzbasic/zzbasic.git
cd zzbasic/0.5.3

# Compile
make

# Execute
./zzbasic
```

#### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.5.3
gcc -o zzbasic *.c
zzbasic.exe
```

---

## Capítulo 2: Conceitos Básicos

### REPL vs. Scripts

O ZzBasic pode ser usado de duas maneiras:

1. **REPL (Read-Eval-Print Loop):** Um ambiente interativo para testar ideias rapidamente. Execute `zzbasic` sem argumentos para entrar no REPL.
2. **Scripts:** Arquivos de texto com extensão `.zz` que contêm uma sequência de comandos. Execute `zzbasic <nome_do_arquivo>.zz` para rodar um script.

### Comentários

Comentários começam com `#` e vão até o final da linha. Eles são ignorados pelo interpretador e servem para documentar seu código.

```basic
# Este é um comentário
let x = 10  # Este também é um comentário
```

Use comentários para explicar o que seu código faz. Isso ajuda você e outras pessoas a entender o programa depois.

### Tipos de Dados

O ZzBasic suporta três tipos de dados principais:

- **Números:** Números de ponto flutuante de dupla precisão (ex: `10`, `3.14`, `-5`).
- **Strings:** Sequências de caracteres entre aspas duplas (ex: `"Olá Mundo"`). O tamanho máximo de uma string ZzBasic é 255 caracteres.
- **Booleanos:** Valores verdadeiro ou falso (`true` ou `false`).

Você não precisa declarar o tipo de uma variável. O ZzBasic descobre automaticamente qual tipo é.

---

## Capítulo 3: Variáveis e Expressões

### Criando Variáveis com `let`

O comando `let` é usado para criar e atribuir valores a variáveis. Uma variável é um espaço na memória que armazena um valor.

```basic
let nome = "Zurg"
let idade = 120
let pi = 3.14
let ativo = true
```

Os nomes de variáveis devem começar com uma letra ou `_` e podem conter letras, números e `_`. Use nomes descritivos para tornar seu código mais legível.

```basic
let nome_completo = "João Silva"  # Bom
let n = "João Silva"              # Ruim - nome não descritivo
```

### Expressões Aritméticas

O ZzBasic suporta as quatro operações aritméticas básicas:

- `+` (adição)
- `-` (subtração)
- `*` (multiplicação)
- `/` (divisão)

```basic
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

```basic
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

```basic
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

```basic
let resultado = 2 + 3 * 4  # Resultado é 14, não 20
let resultado = (2 + 3) * 4  # Resultado é 20
let resultado = 5 > 3 and 2 < 4  # true
```

Use parênteses para deixar claro qual operação deve ser feita primeiro.

---

## Capítulo 4: Comando `print`

O comando `print` é usado para exibir valores na tela. Ele tem uma sintaxe flexível e poderosa.

### Imprimindo Valores Simples

```basic
print "Olá Mundo"
print 42
print 3.14
print true
```

### Imprimindo Variáveis

```basic
let nome = "Zurg"
print nome
```

### Imprimindo Múltiplos Valores

Você pode imprimir vários valores em um único comando. Eles são separados por espaços automaticamente.

```basic
let nome = "Zurg"
let idade = 120
print "Nome:" nome
print "Idade:" idade
print "Resultado:" 10 + 5
```

Note que para imprimir vários valores no mesmo print estes devem estar separados por um espaço em branco.

### Imprimindo Expressões

Você pode imprimir o resultado de expressões diretamente:

```basic
print 5 > 3 nl           # true
print true and false nl  # false
print 10 + 5 * 2 nl      # 20
```

### Controle de Nova Linha (`nl`)

Por padrão, o `print` **não** quebra a linha. Para quebrar a linha, use `nl` no final do comando.

```basic
print "Olá"  # Fica na mesma linha
print "Mundo" nl # Quebra a linha
```

Isso é útil quando você quer construir uma linha gradualmente:

```basic
print "Progresso: ["
print "."
print "."
print "."
print "]" nl
```

### Atalho `?`

O `?` é um atalho para `print`. Use quando quiser digitar menos:

```basic
? "Teste rápido" nl
? 10 + 20 nl
? true nl
```

### Cores e Formatação

O `print` suporta 24 cores para tornar a saída mais expressiva e legível.

#### Cores Normais

```basic
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print green "SUCESSO" nocolor " - Operação concluída" nl
print yellow "AVISO" nocolor ": Verifique os dados" nl
print blue "Informação" nocolor ": Leia com atenção" nl
```

#### Cores Brilhantes

```basic
print bred "CRÍTICO" nocolor " - Sistema em risco" nl
print bgreen "OK" nocolor " - Tudo funcionando" nl
```

#### Cores de Fundo

```basic
print bgred "ALERTA" nocolor " - Ação necessária" nl
print bgblue white "INFORMAÇÃO" nocolor " - Leia com atenção" nl
```

#### Combinações

```basic
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

```basic
print width(20) "Nome" nl
print width(10) 42 nl
```

#### Alinhamento

Use `left`, `right` ou `center` para controlar o alinhamento dentro do campo:

```basic
print width(20) left "Esquerda" nl
print width(20) right "Direita" nl
print width(20) center "Centro" nl
```

#### Exemplos Práticos

**Tabela Simples:**

```basic
print width(20) left "Produto" " | " width(10) right "Preço" nl
print width(20) left "Notebook" " | " width(10) right 4500.50 nl
print width(20) left "Mouse" " | " width(10) right 89.90 nl
```

**Combinando com Cores:**

```basic
print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
print yellow width(40) center "Verifique as permissões" nocolor nl
```

**Tabela Formatada:**

```basic
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "============================================" nocolor nl
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
```

#### Função `repeat()` (Futura)

Nas próximas versões, será possível usar a função `repeat(char, n)` para criar linhas facilmente:

```basic
# Futura sintaxe (v0.6.0)
print repeat("=", 50) nl
print repeat("-", 30) nl
```

Por enquanto, use strings com caracteres repetidos manualmente.

---

## Capítulo 5: Entrada de Dados

### Comando `input`

O comando `input` é usado para ler uma linha de texto do usuário. A sintaxe é:

```basic
input "mensagem" variavel
```

**Exemplos:**

```basic
input "Digite seu nome: " nome
print "Olá, " nome nl
```

```basic
input "Digite um número: " numero
print "Você digitou: " numero nl
```

```basic
input "Digite sua idade: " idade

if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

---

## Capítulo 6: Controle de Fluxo

### if...else

O comando `if` é usado para executar código condicionalmente:

```basic
let idade = 18

if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

### else if

Você pode usar múltiplos `else if` para testar várias condições:

```basic
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

### while

O comando `while` é usado para repetir um bloco de código enquanto uma condição for verdadeira:

```basic
let i = 0
while (i < 5) do
    print i nl
    let i = i + 1
end while
```

### break

O comando `break` é usado para sair de um loop `while` imediatamente:

```basic
let i = 0
while (i < 10) do
    if (i == 5) then
        break
    end if
    print i nl
    let i = i + 1
end while
```

### continue

O comando `continue` é usado para pular para a próxima iteração de um loop `while`:

```basic
let i = 0
while (i < 5) do
    let i = i + 1
    if (i == 3) then
        continue
    end if
    print i nl
end while
```

---

## Capítulo 7: Exemplos Práticos

### Exemplo 1: Cálculo de Média

```basic
# Cálculo de média de notas
let nota1 = 8.5
let nota2 = 7.5
let nota3 = 9.0
let media = (nota1 + nota2 + nota3) / 3

print "Notas:" nota1 nota2 nota3 nl
print "Média:" media nl

if (media >= 7) then
    print green "Aprovado" nocolor nl
else
    print red "Reprovado" nocolor nl
end if
```

### Exemplo 2: Validação de Idade

```basic
print "Digite sua idade: " nl
input "Idade: " idade

if (idade >= 18) then
    print green "Maior de idade" nocolor nl
else if (idade >= 13) then
    print yellow "Adolescente" nocolor nl
else
    print blue "Criança" nocolor nl
end if
```

### Exemplo 3: Jogo de Adivinhar o Número

```basic
# --- Jogo de Adivinhar o Numero ---
# Demonstração de todos os recursos do ZzBasic v0.5.3

print cyan "=========================================" nocolor nl
print "===   Bem-vindo ao Jogo de Adivinha   ===" nl
print cyan "=========================================" nocolor nl

# --- Configuracoes do Jogo ---
let secret_number = 42
let max_attempts = 7
let attempts_left = max_attempts
let guessed_correctly = 0

print "Eu pensei em um numero entre 1 e 100." nl
print "Voce tem " max_attempts " tentativas para adivinhar." nl
print "" nl

# --- Loop Principal do Jogo ---
while (attempts_left > 0) do
    input "Qual e o seu palpite? " guess

    # --- Validacao do Palpite (demonstra 'continue') ---
    if (guess < 1 or guess > 100) then
        print magenta "-> Palpite invalido! Por favor, digite um numero entre 1 e 100." nocolor nl
        continue
    end if

    # --- Decrementa as tentativas ---
    let attempts_left = attempts_left - 1

    # --- Comparacao do Palpite (demonstra 'if/else if/else') ---
    if (guess == secret_number) then
        let guessed_correctly = 1
        break
    else if (guess < secret_number) then
        print blue "-> Muito baixo!" nocolor nl
    else
        print red "-> Muito alto!" nocolor nl
    end if

    # --- Mostra tentativas restantes ---
    if (attempts_left > 0) then
        print yellow "Voce ainda tem " attempts_left " tentativas." nocolor nl
    end if
end while

# --- Fim de Jogo ---
print "" nl
print cyan "=========================================" nocolor nl

if (guessed_correctly == 1) then
    # --- Mensagem de Vitoria ---
    print green "*** PARABENS! VOCE ACERTOU! ***" nocolor nl
    print "O numero secreto era " secret_number "." nl
else
    # --- Mensagem de Derrota ---
    print red "--- FIM DE JOGO! VOCE PERDEU! ---" nocolor nl
    print "O numero secreto era " secret_number "." nl
end if

print cyan "=========================================" nocolor nl
print "Obrigado por jogar!" nl
```

---

## Capítulo 8: Próximos Passos (v0.5.4)

- **Loop `for`:** Uma nova maneira de criar loops com mais controle.
- **`input` com formatação:** Adicionar cores, largura e alinhamento ao comando `input`.
- **REPL multi-linha:** Permitir a escrita de blocos de código com várias linhas no REPL.

---

## 📜 Licença

Este projeto é licenciado sob a Licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

---

arataca89@gmail.com


