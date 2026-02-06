# Manual do Usuário - ZzBasic v0.5.2

![ZzBasic](zzbasic_logo1.png)

**Bem-vindo ao ZzBasic!** Este manual é o seu guia completo para aprender e dominar a linguagem de programação ZzBasic. Aqui você encontrará tudo o que precisa para começar, desde a instalação até a criação de programas úteis.

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
cd zzbasic/0.5.2

# Compile
make

# Execute
./zzbasic
```

#### Compilação (Windows)

```bash
# Com MinGW
cd zzbasic\0.5.2
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
Note que para imprimir vários valores no mesmo `print` estes devem estar separados por um espaço em branco.
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

---

## Capítulo 5: Entrada de Dados

### Comando `input`

O comando `input` é usado para ler uma linha de texto do usuário:

```basic
input "Digite seu nome: " 
print "Olá, " nome nl
```

### Exemplo Prático

```basic
input "Digite um número: " 
print "Você digitou: " numero nl
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

### Exemplos com Operadores Lógicos

```basic
let idade = 25
let tem_carteira = true

if (idade >= 18 and tem_carteira) then
    print "Pode dirigir" nl
else
    print "Não pode dirigir" nl
end if
```

```basic
let chovendo = true
let frio = false

if (chovendo or frio) then
    print "Leve um casaco" nl
end if
```

```basic
let numero = 10

if (not (numero < 0)) then
    print "Número é positivo ou zero" nl
end if
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
let idade = input()

if (idade >= 18) then
    print green "Maior de idade" nocolor nl
else if (idade >= 13) then
    print yellow "Adolescente" nocolor nl
else
    print blue "Criança" nocolor nl
end if
```

### Exemplo 3: Comparação de Números

```basic
let x = 10
let y = 5
let z = 10

print "X = " x nl
print "Y = " y nl
print "Z = " z nl

if (x == z) then
    print "X é igual a Z" nl
end if

if (x > y) then
    print "X é maior que Y" nl
end if

if (y != z) then
    print "Y é diferente de Z" nl
end if
```

### Exemplo 4: Mensagem com Cores

```basic
# Mensagem com cores
let status = "OK"
print green "Status:" nocolor status nl
print blue "Sistema operacional" nocolor nl
```

### Exemplo 5: Construindo uma Linha

```basic
# Construindo uma linha progressivamente
print "["
print "="
print "="
print "="
print "]" nl
```

---

## Capítulo 8: Comandos Úteis no REPL

O REPL oferece alguns comandos especiais para ajudá-lo a debugar e entender seu código.

### `help`

Exibe uma mensagem de ajuda:

```basic
>> help
```

### `tokens`

Mostra como o ZzBasic interpreta sua entrada (análise léxica):

```basic
>> tokens print "Olá" 10 + 5 nl
=== LEXICAL ANALYSIS ===
  1: (1:1)[PRINT]: print
  2: (1:7)[STRING]: "Olá"
  3: (1:12)[NUMBER]: 10
  4: (1:15)[PLUS]: +
  5: (1:17)[NUMBER]: 5
  6: (1:19)[NL]: nl
  7: (1:21)[EOF]
=== END OF ANALYSIS ===
```

### `ast`

Mostra a Árvore Sintática Abstrata (AST) de sua entrada:

```basic
>> ast let x = 10 + 5
AST for: "let x = 10 + 5"
[1:1] STATEMENT_LIST (1 statements)
    [1:1] ASSIGNMENT: x
        [1:9] BINARY_OP: +
            [1:9] NUMBER: 10
            [1:13] NUMBER: 5
```

### `exit` e `quit`

Sai do REPL:

```basic
>> exit
```

ou

```basic
>> quit
```

---

## Apêndice A: Referência de Comandos

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `let` | Cria e atribui um valor a uma variável. | `let x = 10` |
| `print` | Exibe um ou mais valores na tela. | `print "Olá" x nl` |
| `?` | Atalho para `print`. | `? "Teste" nl` |
| `input` | Lê uma linha de texto do usuário. | `input "Entre com um número: "` |
| `if...then...else...end if` | Executa código condicionalmente. |[1] |
| `exit` | Sai do REPL. | `exit` |
| `quit` | Sai do REPL. | `quit` |
| `help` | Exibe ajuda no REPL. | `help` |
| `tokens` | Exibe a análise léxica no REPL. | `tokens print "Olá"` |
| `ast` | Exibe a AST no REPL. | `ast let x = 10` |

[1] Exemplo do uso de `if...else`

```basic
let x = 5
if (x > 3) then
    print "X é maior que 3" nl
end if

if (x > 10) then
    print "X é maior que 10" nl
else
    print "X é menor ou igual a 10" nl
end if
```
---

## Apêndice B: Operadores

### Operadores Aritméticos

| Operador | Descrição | Exemplo |
|----------|-----------|---------|
| `+` | Adição | `10 + 5` = 15 |
| `-` | Subtração | `10 - 5` = 5 |
| `*` | Multiplicação | `10 * 5` = 50 |
| `/` | Divisão | `10 / 5` = 2 |

### Operadores de Comparação

| Operador | Descrição | Exemplo |
|----------|-----------|---------|
| `==` | Igual | `5 == 5` = true |
| `!=` | Diferente | `5 != 3` = true |
| `<` | Menor que | `3 < 5` = true |
| `>` | Maior que | `5 > 3` = true |
| `<=` | Menor ou igual | `5 <= 5` = true |
| `>=` | Maior ou igual | `5 >= 5` = true |

### Operadores Lógicos

| Operador | Descrição | Exemplo |
|----------|-----------|---------|
| `and` | E lógico | `true and true` = true |
| `or` | OU lógico | `true or false` = true |
| `not` | NÃO lógico | `not true` = false |
| `!` | NÃO lógico (atalho) | `! true` = false |

---

## Apêndice C: Dicas e Boas Práticas

### Use Nomes Descritivos

```basic
# Bom
let preco_unitario = 10.50
let quantidade = 5
let total = preco_unitario * quantidade

# Ruim
let p = 10.50
let q = 5
let t = p * q
```

### Use Comentários

```basic
# Calcula o preço total
let preco_unitario = 10.50
let quantidade = 5
let total = preco_unitario * quantidade  # Multiplica preço por quantidade
```

### Use Cores para Destacar Mensagens Importantes

```basic
print green "SUCESSO" nocolor ": Operação concluída" nl
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print yellow "AVISO" nocolor ": Verifique os dados" nl
```

### Quebre Linhas Longas

```basic
# Difícil de ler
print "Nome:" nome "Idade:" idade "Cidade:" cidade "País:" pais nl

# Mais fácil de ler
print "Nome:" nome nl
print "Idade:" idade nl
print "Cidade:" cidade nl
print "País:" pais nl
```

### Use Parênteses para Clareza

```basic
# Menos claro
let resultado = 5 > 3 and 2 < 4

# Mais claro
let resultado = (5 > 3) and (2 < 4)
```

### Teste Expressões no REPL

Use o REPL para testar expressões antes de usá-las em seu código:

```basic
>> 5 > 3 and 2 < 4 nl
true

>> 10 + 5 * 2 nl
20
```

---

## Apêndice D: Mensagens de Erro Comuns

### Erro: "Unexpected factor"

Você provavelmente digitou algo que o ZzBasic não conseguiu entender. Verifique se:

- Todas as aspas estão fechadas: `"texto"`
- Todos os parênteses estão balanceados: `(10 + 5)`
- Você não está usando operadores inválidos: `%`, `&`, etc.

### Erro: "division by zero"

Você tentou dividir um número por zero. Verifique sua expressão:

```basic
let resultado = 10 / 0  # ERRO!
```

### Erro: "incomplete or invalid expression"

Você provavelmente deixou uma expressão incompleta. Verifique se:

- Não há operadores no final: `10 +`
- Não há parênteses abertos: `(10 + 5`

### Erro: "Expected ')' after condition"

Você provavelmente esqueceu de fechar um parêntese em uma condição:

```basic
if (x > 5  # ERRO! Falta fechar )
    print "OK" nl
end if
```

### Erro: "'end' expected"

Você provavelmente esqueceu de fechar um bloco `if`:

```basic
if (x > 5) then
    print "OK" nl
# ERRO! Falta 'end if'
```

---

## Histórico de Versões

| Versão | Data | Novidades |
|--------|------|-----------|
| 0.4.1 | Janeiro 2026 | Versão inicial com print, cores, variáveis |
| 0.5.0 | Fevereiro 2026 | Tipo booleano |
| 0.5.1 | Fevereiro 2026 | Operadores de comparação e lógicos |
| 0.5.2 | Fevereiro 2026 | if...else |

---

**Última atualização:** Fevereiro de 2026  
**Versão:** 0.5.2

---

## Próximas Funcionalidades

As próximas versões do ZzBasic incluirão:

- **v0.5.3:** loop `while` com `break` e `continue`
- **v0.5.4:** loop `for`

---

arataca89@gmail.com


