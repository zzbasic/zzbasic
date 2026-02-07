# Manual do Usuário - ZzBasic v0.5.2

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

```python
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

```python
let nome = "Zurg"
let idade = 120
let pi = 3.14
let ativo = true
```

Os nomes de variáveis devem começar com uma letra ou `_` e podem conter letras, números e `_`. Use nomes descritivos para tornar seu código mais legível.

```python
let nome_completo = "João Silva"  # Bom
let n = "João Silva"              # Ruim - nome não descritivo
```

### Expressões Aritméticas

O ZzBasic suporta as quatro operações aritméticas básicas:

- `+` (adição)
- `-` (subtração)
- `*` (multiplicação)
- `/` (divisão)

```python
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

```python
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

```python
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

```python
let resultado = 2 + 3 * 4  # Resultado é 14, não 20
let resultado = (2 + 3) * 4  # Resultado é 20
let resultado = 5 > 3 and 2 < 4  # true
```

Use parênteses para deixar claro qual operação deve ser feita primeiro.

---

## Capítulo 4: Comando `print`

O comando `print` é usado para exibir valores na tela. Ele tem uma sintaxe flexível e poderosa.

### Imprimindo Valores Simples

```python
print "Olá Mundo"
print 42
print 3.14
print true
```

### Imprimindo Variáveis

```python
let nome = "Zurg"
print nome
```

### Imprimindo Múltiplos Valores

Você pode imprimir vários valores em um único comando. Eles são separados por espaços automaticamente.

```python
let nome = "Zurg"
let idade = 120
print "Nome:" nome
print "Idade:" idade
print "Resultado:" 10 + 5
```

Note que para imprimir vários valores no mesmo print estes devem estar separados por um espaço em branco.

### Imprimindo Expressões

Você pode imprimir o resultado de expressões diretamente:

```python
print 5 > 3 nl           # true
print true and false nl  # false
print 10 + 5 * 2 nl      # 20
```

### Controle de Nova Linha (`nl`)

Por padrão, o `print` **não** quebra a linha. Para quebrar a linha, use `nl` no final do comando.

```python
print "Olá"  # Fica na mesma linha
print "Mundo" nl # Quebra a linha
```

Isso é útil quando você quer construir uma linha gradualmente:

```python
print "Progresso: ["
print "."
print "."
print "."
print "]" nl
```

### Atalho `?`

O `?` é um atalho para `print`. Use quando quiser digitar menos:

```python
? "Teste rápido" nl
? 10 + 20 nl
? true nl
```

### Cores e Formatação

O `print` suporta 24 cores para tornar a saída mais expressiva e legível.

#### Cores Normais

```python
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print green "SUCESSO" nocolor " - Operação concluída" nl
print yellow "AVISO" nocolor ": Verifique os dados" nl
print blue "Informação" nocolor ": Leia com atenção" nl
```

#### Cores Brilhantes

```python
print bred "CRÍTICO" nocolor " - Sistema em risco" nl
print bgreen "OK" nocolor " - Tudo funcionando" nl
```

#### Cores de Fundo

```python
print bgred "ALERTA" nocolor " - Ação necessária" nl
print bgblue white "INFORMAÇÃO" nocolor " - Leia com atenção" nl
```

#### Combinações

```python
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

```python
print width(20) "Nome" nl
print width(10) 42 nl
```

#### Alinhamento

Use `left`, `right` ou `center` para controlar o alinhamento dentro do campo:

```python
print width(20) left "Esquerda" nl
print width(20) right "Direita" nl
print width(20) center "Centro" nl
```

#### Exemplos Práticos

**Tabela Simples:**

```python
print width(20) left "Produto" " | " width(10) right "Preço" nl
print width(20) left "Notebook" " | " width(10) right 4500.50 nl
print width(20) left "Mouse" " | " width(10) right 89.90 nl
```

**Combinando com Cores:**

```python
print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
print yellow width(40) center "Verifique as permissões" nocolor nl
```

**Tabela Formatada:**

```python
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "============================================" nocolor nl
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
```

#### Função `repeat()` (Futura)

Nas próximas versões, será possível usar a função `repeat(char, n)` para criar linhas facilmente:

```python
# Futura sintaxe (v0.6.0)
print repeat('=', 50) nl
print repeat('-', 30) nl
```

Por enquanto, use strings com caracteres repetidos manualmente.

---

## Capítulo 5: Entrada de Dados

### Comando `input`

O comando `input` é usado para ler uma linha de texto do usuário. A sintaxe é:

```python
input "mensagem" variavel
```

**Exemplos:**

```python
input "Digite seu nome: " nome
print "Olá, " nome nl
```

```python
input "Digite um número: " numero
print "Você digitou: " numero nl
```

```python
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

```python
let idade = 18

if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

### else if

Você pode usar múltiplos `else if` para testar várias condições:

```python
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

```python
let idade = 25
let tem_carteira = true

if (idade >= 18 and tem_carteira) then
    print "Pode dirigir" nl
else
    print "Não pode dirigir" nl
end if
```

```python
let chovendo = true
let frio = false

if (chovendo or frio) then
    print "Leve um casaco" nl
end if
```

```python
let numero = 10

if (not (numero < 0)) then
    print "Número é positivo ou zero" nl
end if
```

---

## Capítulo 7: Exemplos Práticos

### Exemplo 1: Cálculo de Média

```python
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

```python
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

### Exemplo 3: Comparação de Números

```python
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

```python
# Mensagem com cores
let status = "OK"
print green "Status:" nocolor status nl
print blue "Sistema operacional" nocolor nl
```

### Exemplo 5: Construindo uma Linha

```python
# Construindo uma linha progressivamente
print "["
print "="
print "="
print "="
print "]" nl
```

---

## Capítulo 8: Script de Teste Completo

Aqui está um script completo que testa todas as funcionalidades implementadas até a v0.5.2:

```python
# ============================================================
# ZzBasic v0.5.2 - Script de Teste Completo
# Testa todas as funcionalidades implementadas até agora
# ============================================================

# Título
print nl
print blue "+============================================================+" nl
print blue "|                  "cyan"ZzBasic v0.5.2                        "blue"|" nl
print blue "+============================================================+" nl
print nl

# ============================================================
# 1. Expressões Aritméticas (v0.1.0)
# ============================================================
nocolor
print "==============================================================" nl
print yellow "1. EXPRESSÕES ARITMÉTICAS" nl
nocolor
print "==============================================================" nl
print nl

print "2 + 3 = " 2 + 3 nl
print "10 - 4 = " 10 - 4 nl
print "5 * 6 = " 5 * 6 nl
print "20 / 4 = " 20 / 4 nl
print "-5 + 10 = " -5 + 10 nl
print "(2 + 3) * 4 = " (2 + 3) * 4 nl
print "10 / (2 + 3) = " 10 / (2 + 3) nl
print nl

# ============================================================
# 2. Variáveis e Comando LET (v0.2.0)
# ============================================================

print "==============================================================" nl
print yellow "2. VARIÁVEIS E COMANDO LET" nl
nocolor
print "==============================================================" nl
print nl
nocolor

let pi = 3.14159
let raio = 10
let area = pi * (raio * raio)

print "Valor de pi: " pi nl
print "Raio: " raio nl
print "Área do círculo: " area nl
print nl

let nome = "ZzBasic"
let versao = "0.5.2"
print "Linguagem: " nome " " versao nl
print nl

# ============================================================
# 3. Comando PRINT (v0.4.0)
# ============================================================

print "==============================================================" nl
print yellow "3. COMANDO PRINT" nl
nocolor
print "==============================================================" nl
print nl

print "Esta linha não pula para a próxima."
print " Esta continua na mesma linha." nl
print "Esta linha pula para a próxima." nl
print nl

# ============================================================
# 4. Cores (v0.4.1)
# ============================================================

print "==============================================================" nl
print yellow "4. CORES" nl
nocolor
print "==============================================================" nl
print nl

print red "ERRO:" nocolor " Arquivo não encontrado." nl
print green "SUCESSO:" nocolor " Operação concluída." nl
print yellow "AVISO:" nocolor " Verifique os dados." nl
print blue "INFO:" nocolor " Leia com atenção." nl
print nl

# ============================================================
# 5. Formatação (v0.4.2)
# ============================================================

print "==============================================================" nl
print yellow "5. FORMATAÇÃO (WIDTH E ALINHAMENTO)" nl
nocolor
print "==============================================================" nl
print nl

# Tabela de Produtos
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "============================================================" nocolor nl
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
print nl

# Combinação de Cores e Formatação
print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
print yellow width(40) center "Verifique as permissões do sistema" nocolor nl
print nl

# ============================================================
# 6. Operadores de Comparação e Lógicos (v0.5.1)
# ============================================================

print "==============================================================" nl
print yellow "6. OPERADORES DE COMPARAÇÃO E LÓGICOS" nl
nocolor
print "==============================================================" nl
print nl

print "5 == 5: " 5 == 5 nl
print "5 != 3: " 5 != 3 nl
print "3 < 5: " 3 < 5 nl
print "5 > 3: " 5 > 3 nl
print "5 <= 5: " 5 <= 5 nl
print "5 >= 5: " 5 >= 5 nl
print nl

print "true and true: " true and true nl
print "true or false: " true or false nl
print "not true: " not true nl
print "! false: " ! false nl
print nl

# ============================================================
# 7. Entrada de Dados (v0.5.0)
# ============================================================

print "==============================================================" nl
print yellow "7. ENTRADA DE DADOS" nl
nocolor
print "==============================================================" nl
print nl

input "Digite seu nome: " seu_nome
print "Olá, " seu_nome "!" nl
print nl

# ============================================================
# 8. Controle de Fluxo - IF...ELSE (v0.5.2)
# ============================================================

print "==============================================================" nl
print yellow "8. CONTROLE DE FLUXO - IF...ELSE" nl
nocolor
print "==============================================================" nl
print nl

let idade = 25

if (idade >= 18) then
    print green "Você é maior de idade" nocolor nl
else
    print red "Você é menor de idade" nocolor nl
end if

print nl

let nota = 7.5

if (nota >= 9) then
    print "Conceito: A (Excelente)" nl
else if (nota >= 7) then
    print "Conceito: B (Bom)" nl
else if (nota >= 5) then
    print "Conceito: C (Satisfatório)" nl
else
    print "Conceito: D (Insuficiente)" nl
end if

print nl

# ============================================================
# 9. Finalização
# ============================================================

print green "Todos os testes foram concluídos com sucesso!" nocolor nl
print nl

# Finalização
print nl
print blue "+============================================================+" nl
print blue "|                  "cyan"FIM DO TESTE                          "blue"|" nl
print blue "+============================================================+" nl
print nl

nocolor
```

---

## Capítulo 9: Comandos Úteis no REPL

O REPL oferece alguns comandos especiais para ajudá-lo a debugar e entender seu código.

### `help`

Exibe uma mensagem de ajuda:

```python
>> help
```

### `tokens`

Mostra como o ZzBasic interpreta sua entrada (análise léxica):

```python
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

```python
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

```python
>> exit
```

ou

```python
>> quit
```

---

## Apêndice A: Referência de Comandos

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `let` | Cria e atribui um valor a uma variável. | `let x = 10` |
| `print` | Exibe um ou mais valores na tela. | `print "Olá" x nl` |
| `?` | Atalho para `print`. | `? "Teste" nl` |
| `input` | Lê uma linha de texto do usuário. | `input "Nome: " nome` |
| `if...then...else...end if` | Executa código condicionalmente. | `if (x > 3) then print "OK" nl end if` |
| `width(n)` | Define largura de campo. | `print width(20) "Texto"` |
| `left` | Alinha à esquerda. | `print width(20) left "Texto"` |
| `right` | Alinha à direita. | `print width(20) right 42` |
| `center` | Alinha ao centro. | `print width(20) center "Texto"` |
| `exit` | Sai do REPL. | `exit` |
| `quit` | Sai do REPL. | `quit` |
| `help` | Exibe ajuda no REPL. | `help` |
| `tokens` | Exibe a análise léxica no REPL. | `tokens print "Olá"` |
| `ast` | Exibe a AST no REPL. | `ast let x = 10` |

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

```python
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

```python
# Calcula o preço total
let preco_unitario = 10.50
let quantidade = 5
let total = preco_unitario * quantidade  # Multiplica preço por quantidade
```

### Use Cores para Destacar Mensagens Importantes

```python
print green "SUCESSO" nocolor ": Operação concluída" nl
print red "ERRO" nocolor ": Arquivo não encontrado" nl
print yellow "AVISO" nocolor ": Verifique os dados" nl
```

### Use Width e Alinhamento para Tabelas

```python
# Tabela bem formatada
print width(20) left "Nome" " | " width(10) right "Idade" nl
print width(20) left "João" " | " width(10) right 25 nl
print width(20) left "Maria" " | " width(10) right 30 nl
```

### Quebre Linhas Longas

```python
# Difícil de ler
print "Nome:" nome "Idade:" idade "Cidade:" cidade "País:" pais nl

# Mais fácil de ler
print "Nome:" nome nl
print "Idade:" idade nl
print "Cidade:" cidade nl
print "País:" pais nl
```

### Use Parênteses para Clareza

```python
# Menos claro
let resultado = 5 > 3 and 2 < 4

# Mais claro
let resultado = (5 > 3) and (2 < 4)
```

### Teste Expressões no REPL

Use o REPL para testar expressões antes de usá-las em seu código:

```python
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

```python
let resultado = 10 / 0  # ERRO!
```

### Erro: "incomplete or invalid expression"

Você provavelmente deixou uma expressão incompleta. Verifique se:

- Não há operadores no final: `10 +`
- Não há parênteses abertos: `(10 + 5`

### Erro: "Expected ')' after condition"

Você provavelmente esqueceu de fechar um parêntese em uma condição:

```python
if (x > 5  # ERRO! Falta fechar )
    print "OK" nl
end if
```

### Erro: "'end' expected"

Você provavelmente esqueceu de fechar um bloco `if`:

```python
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
| 0.5.2 | Fevereiro 2026 | if...else com else if |

---

**Última atualização:** Fevereiro de 2026  
**Versão:** 0.5.2

---

## Próximas Funcionalidades

As próximas versões do ZzBasic incluirão:

- **v0.5.3:** loop `while` com `break` e `continue`
- **v0.5.4:** loop `for`
- **v0.5.5:** Arrays 
- **v0.5.6:** Matrizes
- **v0.6.0:** Funções/Procedures e função `repeat()`
