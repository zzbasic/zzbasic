# Exemplos ZzBasic v0.6.0

## 1. hello_world.zz - Seu Primeiro Programa

O clássico "Olá, Mundo!":

```basic
print "Olá, Mundo!" nl
```

---

## 2. variaveis.zz - Trabalhando com Variáveis

Aprenda a criar e usar variáveis:

```basic
let nome = "Alice"
let idade = 25
let altura = 1.65

print "Nome: " nome nl
print "Idade: " idade nl
print "Altura: " altura nl
```

---

## 3. operacoes_aritmeticas.zz - Cálculos Matemáticos

Operações básicas com números:

```basic
let a = 10
let b = 3

print "Soma: " a + b nl
print "Subtração: " a - b nl
print "Multiplicação: " a * b nl
print "Divisão: " a / b nl
```

---

## 4. comparacoes.zz - Comparando Valores

Entenda operadores de comparação:

```basic
let x = 10
let y = 20

print "x == y: " x == y nl
print "x != y: " x != y nl
print "x < y: " x < y nl
print "x > y: " x > y nl
print "x <= y: " x <= y nl
print "x >= y: " x >= y nl
```

---

## 5. if_else.zz - Controle Condicional

Tome decisões no seu programa:

```basic
let idade = 20

if (idade >= 18) then
    print "Você é maior de idade" nl
else
    print "Você é menor de idade" nl
end if
```

---

## 6. loop_while.zz - Loop while 

Repita ações enquanto uma condição for verdadeira:

```basic
let i = 1
while (i <= 5) do
    print "Iteração " i nl
    let i = i + 1
end while
```

---

## 7. loop_for.zz - Loop for - quando você sabe o número de repetições

Repita um número específico de vezes:

```basic
for i = 1 to 5 do
    print "Número: " i nl
end for
```

---

## 8. arrays_basico.zz - Introdução a Arrays

Trabalhe com coleções de dados:

```basic
let numeros = array(5)
push(numeros, 10)
push(numeros, 20)
push(numeros, 30)

print "Primeiro: " numeros[0] nl
print "Segundo: " numeros[1] nl
print "Terceiro: " numeros[2] nl
print "Tamanho: " len(numeros) nl
```

---

## 9. entrada_saida.zz - Interação com Usuário

Leia dados do usuário:

```basic
input "Digite seu nome: " nome
input "Digite sua idade: " idade

print "Olá, " nome "!" nl
print "Você tem " idade " anos" nl
```

---

## 10. cores_formatacao.zz - Cores e Formatação

Use cores e formatação na saída:

```basic
print red "Texto vermelho" nl
print green "Texto verde" nl
print blue "Texto azul" nl
nocolor # reseta a cor
print nl

print width(30) right "Texto alinhado" nl
print width(30) center "Texto centralizado" nl
print nl

print red "Vermelho" nocolor " normal" nl
```

---

## 11. calculadora_imc.zz - Calculadora de IMC

Calcule o Índice de Massa Corporal:

```basic
# Entrada de dados
input "Digite seu peso (kg) : " peso
input "Digite sua altura (m): " altura

# Cálculo
let imc = peso / (altura * altura)

# Resultado
print "Seu IMC é: " imc nl

if (imc < 18.5) then
    print "Abaixo do peso" nl
else
    if (imc < 25) then
        print "Peso normal" nl
    else
        if (imc < 30) then
            print "Sobrepeso" nl
        else
            print "Obesidade" nl
        end if
    end if
end if
```

---

## 12. tabuada.zz - Gerador de Tabuada

Gere a tabuada de qualquer número:

```basic
input "Digite um número: " numero

print "Tabuada do " numero ": " nl

for i = 1 to 10 do
    let resultado = numero * i
    print numero " x" i " =" resultado nl
end for
```

---

## 13. fibonacci.zz - Sequência de Fibonacci

Gere os primeiros N números de Fibonacci:

```basic
input "Quantos números de Fibonacci? " n

let a = 0
let b = 1

print a nl
if (n > 1) then
    print b nl
end if

let i = 2
while (i < n) do
    let c = a + b
    print c nl
    let a = b
    let b = c
    let i = i + 1
end while
```

---

## 14. gerenciador_notas.zz - Gerenciador de Notas

Armazene e analise notas de alunos:

```basic
# Entrada de notas
print "Digite as notas (digite -1 para parar):" nl
let i = 1
let nota = 0
let notas = array(0)

while (nota != -1) do
    print "Nota" i 
    input ": " nota
    if (nota == -1) then
        break
    end if
    push(notas, nota)
    let i = i + 1
end while

print notas nl

# Calcular estatísticas
let soma = 0
let i = 0
while (i < len(notas)) do
    let soma = soma + notas[i]
    let i = i + 1
end while

let media = soma / len(notas)

# Exibir resultados
print "Média         : " media nl
print "Total de notas: " len(notas) nl
```

OBSERVAÇÃO: `input` só lê uma valor por vez.

```
# CORRETO
input "n: " n 
print n nl

# ERRO
input "n: " n "m: " m
print n "," m nl
```

---

## 15. conversor_temperaturas.zz - Conversor de Temperaturas

Converta entre Celsius e Fahrenheit:

```basic
print "Conversor de Temperaturas" nl
print "1. Celsius para Fahrenheit" nl
print "2. Fahrenheit para Celsius" nl
input "Escolha (1 ou 2): " opcao

if (opcao == 1) then
    input "Digite a temperatura em Celsius: " celsius
    let fahrenheit = (celsius * 9 / 5) + 32
    print celsius "°C = " fahrenheit "°F" nl
else
    if (opcao == 2) then
        input "Digite a temperatura em Fahrenheit: " fahrenheit
        let celsius = (fahrenheit - 32) * 5 / 9
        print fahrenheit "°F = " celsius "°C" nl
    else
        print "Opção inválida" nl
    end if
end if
```

---

## 16. calculadora_desconto.zz - Calculadora de Desconto

Calcule preços com desconto:

```basic
input "Digite o preço original: " preco
input "Digite o percentual de desconto: " percentual

let desconto = preco * (percentual / 100)
let preco_final = preco - desconto

print "Preço original: R$ " preco nl
print "Desconto (" percentual "%): R$ " desconto nl
print "Preço final: R$ " preco_final nl
```

---

## 17. calculadora_juros.zz - Calculadora de Juros Compostos

Calcule juros compostos:

```basic
input "Capital inicial: " capital
input "Taxa de juros anual (%): " taxa
input "Número de anos: " anos

let montante = capital
let i = 0

while (i < anos) do
    let montante = montante * (1 + taxa / 100)
    let i = i + 1
end while

print "Capital inicial: R$ " capital nl
print "Taxa: " taxa "% a.a." nl
print "Período: " anos " anos" nl
print "Montante final: R$ " montante nl
```

---

## 18. guess_game.zz - Jogo de Adivinhar 

Um jogo completo de adivinhar números com feedback detalhado:

```basic
# --- Jogo de Adivinhar o Número ---

print cyan "=========================================" nocolor nl
print "===   Bem-vindo ao Jogo de Adivinha   ===" nl
print cyan "=========================================" nocolor nl

# --- Configurações do Jogo ---
let secret_number = 42
let max_attempts = 7
let attempts_left = max_attempts
let guessed_correctly = 0

print "Eu pensei em um número entre 1 e 100." nl
print "Você tem " max_attempts " tentativas para adivinhar." nl
print "" nl

# --- Loop Principal do Jogo ---
while (attempts_left > 0) do
    input "Qual é o seu palpite? " guess

    # --- Validação do Palpite ---
    if (guess < 1 or guess > 100) then
        print magenta "-> Palpite inválido! Por favor,"
        print " digite um número entre 1 e 100." nocolor nl
        continue
    end if

    # --- Decrementa as tentativas ---
    let attempts_left = attempts_left - 1

    # --- Comparação do Palpite ---
    if (guess == secret_number) then
        let guessed_correctly = 1
        break
    else
        if (guess < secret_number) then
            print blue "-> Muito baixo!" nocolor nl
        else
            print red "-> Muito alto!" nocolor nl
        end if
    end if

    # --- Mostra tentativas restantes ---
    if (attempts_left > 0) then
        print yellow "Você ainda tem " attempts_left " tentativas." nocolor nl
    end if
end while

# --- Fim de Jogo ---
print "" nl
print cyan "=========================================" nocolor nl

if (guessed_correctly == 1) then
    # --- Mensagem de Vitória ---
    print green "*** PARABÉNS! VOCÊ ACERTOU! ***" nocolor nl
    print "O número secreto era " secret_number "." nl
else
    # --- Mensagem de Derrota ---
    print red "--- FIM DE JOGO! VOCÊ PERDEU! ---" nocolor nl
    print "O número secreto era " secret_number "." nl
end if

print cyan "=========================================" nocolor nl
print "Obrigado por jogar!" nl
```



