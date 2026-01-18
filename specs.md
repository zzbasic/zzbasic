# 📘 Especificação da Linguagem ZzBasic 

## 1. Visão Geral
ZzBasic é uma linguagem interpretada, de tipagem dinâmica, que prioriza a **legibilidade** e **acessibilidade** para iniciantes, mantendo a expressividade de linguagens modernas. Sua sintaxe é inspirada no BASIC clássico, mas com semântica ampliada por conceitos de Python e C.

## 2. Tipos de Dados

### 2.1 Tipos Primitivos
| Tipo | Descrição | Exemplo Literal |
| :--- | :--- | :--- |
| **`number`** | Número de ponto flutuante de precisão dupla (IEEE 754). Não há distinção sintaxe entre inteiro e decimal. | `42`, `-7`, `3.14` |
| **`string`** | Cadeia de caracteres imutável, com **tamanho máximo fixo de 256 caracteres**. Ideal para mensagens de interface e dados pequenos. | `"Olá"`, `"A"` |
| **`text`** | Cadeia de caracteres mutável, com tamanho limitado apenas pela memória. Usada para manipulação dinâmica de texto. | `t = "Um texto longo..."` |
| **`bool`** | Valor booleano. | `true`, `false` |

### 2.2 Tipos Estruturados (Complexos)
| Tipo | Descrição | Analogia / Inspiração |
| :--- | :--- | :--- |
| **`list`** | Coleção ordenada e mutável de elementos de **qualquer tipo**. Suporta indexação e métodos como `.append()`, `.pop()`. | Python List |
| **`dict`** | Coleção de pares chave-valor, onde a chave é uma `string`. | Python Dict |
| **`stack`** | Estrutura **LIFO** (Last-In, First-Out) especializada com operações `.push()` e `.pop()`. | Estrutura de dados clássica |
| **`struct`** | Agregado de dados para criar **tipos personalizados**. Funciona como uma **classe simplificada do Python**, podendo conter métodos. | Python Class (simplificada) |

## 3. Sintaxe Básica e Comandos

### 3.1 Comandos de E/S (Entrada/Saída)
```basic
print "Olá, mundo!"  // Saída padrão
print red, "Erro: ", mensagem // Saída com cor

input "Digite seu nome: ", nome // Lê do teclado
input #arquivo, dado // Lê de um arquivo
```

### 3.2 Controle de Fluxo
```basic
// IF...THEN...ELSE (estilo BASIC moderno)
if x > 10 then
    print "Maior que dez"
elseif x == 10 then
    print "Igual a dez"
else
    print "Menor que dez"
end if

// Laço FOR (estilo Python)
for i = 1 to 10 step 2
    print i
next i

// Laço WHILE
while x > 0
    x = x - 1
end while
```

### 3.3 Operadores
*   **Aritméticos:** `+`, `-`, `*`, `/`, `%` (módulo), `^` (potenciação)
*   **Comparação:** `==`, `!=`, `<`, `>`, `<=`, `>=`
*   **Lógicos:** `and`, `or`, `not`
*   **Precedência:** Parênteses `( )` alteram a prioridade padrão.



## 🎨 4. Sistema de Cores ANSI no Terminal

ZzBasic oferece suporte nativo a cores no terminal através de **códigos de escape ANSI**, integrados de forma intuitiva ao comando `print`.

### 4.1 Sintaxe
As cores são especificadas **antes** da string a ser exibida, usando palavras-chave predefinidas que representam os códigos ANSI.

```basic
print red, "Este texto será vermelho"
print green, "Sucesso!", blue, " E isto será azul"
print bold, cyan, "Texto em ciano e negrito"
print color_reset, "Texto volta ao normal"
```

**Regra:** O comando `print` processa os argumentos em sequência. Quando encontra um identificador de cor ou estilo, ele altera o estado de saída até o próximo comando `print` ou até encontrar `color_reset`.

### 4.2 Cores e Estilos Suportados
A linguagem suporta um conjunto rico baseado na paleta ANSI de 8/16 cores e estilos comuns.

#### Cores de Texto (Primeiro Plano)
```basic
black, red, green, yellow, blue, magenta, cyan, white
bright_black, bright_red, bright_green, bright_yellow
bright_blue, bright_magenta, bright_cyan, bright_white
```

#### Cores de Fundo
```basic
bg_black, bg_red, bg_green, bg_yellow, bg_blue, bg_magenta, bg_cyan, bg_white
bg_bright_black, bg_bright_red, bg_bright_green, bg_bright_yellow
bg_bright_blue, bg_bright_magenta, bg_bright_cyan, bg_bright_white
```

#### Estilos de Texto (Modificadores)
```basic
bold       // Negrito (ou intenso)
dim        // Baixa intensidade
italic     // Itálico (nem todos os terminais suportam)
underline  // Sublinhado
blink      // Piscante (use com moderação!)
reverse    // Inverte cor do texto e do fundo
hidden     // Texto invisível (útil para senhas)
```

### 4.3 Comandos Especiais de Controle
```basic
color_reset    // Restaura todas as configurações para o padrão do terminal
color_default  // Restaura apenas a cor do texto para o padrão
bg_default     // Restaura apenas a cor do fundo para o padrão
```

### 4.4 Exemplos Práticos
```basic
// Mensagem de erro típica
print bold, bright_white, bg_red, " ERRO ", color_reset, red, " Arquivo não encontrado."
// Saída: 🟥 ERRO Arquivo não encontrado. (em vermelho)

// Mensagem de sucesso
print green, "✓ Tarefa concluída com sucesso!"

// Título destacado
print bold, underline, cyan, "=== MENU PRINCIPAL ===", color_reset

// Texto com múltiplos estilos
print bold, yellow, "Aviso: ", color_reset, underline, "Esta operação não pode ser desfeita."

// Progresso colorido (em um loop)
for i = 1 to 10
    if i <= 3 then
        print red, "Fase crítica: ", i
    elseif i <= 7 then
        print yellow, "Processando: ", i
    else
        print green, "Concluindo: ", i
    end if
next i
```

### 4.5 Sugestões de Implementação para o Interpretador

1.  **Tabela de Mapeamento Interna**: Crie um dicionário no interpretador que mapeie palavras como `"red"` para o código de escape ANSI `"\033[31m"`.
2.  **Processamento no `print`**: Ao executar `print`, verifique cada argumento. Se for uma string de cor conhecida, emita o código ANSI correspondente. Se for `color_reset`, emita `"\033[0m"`.
3.  **Detecção de Terminal**: É uma boa prática verificar se a saída padrão é um terminal (`isatty()`) antes de emitir códigos ANSI, para evitar lixo em arquivos de log.
4.  **Funções de Ajuda (Opcional)**: Pode-se adicionar funções para composição:
    ```basic
    function error_msg(texto)
        print bold, bright_white, bg_red, " ERRO ", color_reset, red, " ", texto
    end function
    error_msg("Divisão por zero!")
    ```


## 5. Escopo
*   **`global`**: Variáveis declaradas no corpo principal do programa.
*   **`local`**: Variáveis declaradas dentro de uma função (escopo por função).
*   **`struct`**: Membros (atributos e métodos) definidos dentro de uma `struct`.

## 6. Funções
```basic
// Definição
function soma(a, b)
    local resultado = a + b
    return resultado
end function

// Chamada
total = soma(5, 3)
print total // 8
```

## 7. Módulos
Sistema de módulos inspirado em Python, usando a palavra-chave `import`:
```basic
// Importa todo o módulo 'matematica'
import matematica
print matematica.pi

// Importa um item específico
from matematica import pi, seno
```

## 8. Decisões de Design (Pontos a Definir)

1.  **`number` único vs. `int`/`float`**: Manter `number` é mais simples para iniciantes, mas pode mascarar diferenças importantes. **Sugestão:** Manter `number` como tipo único, mas oferecer funções `int()`, `float()` para conversão explícita.
2.  **Strings (`string` vs `text`)**: A distinção é interessante para otimização. **Sugestão:** Usar `string` para literais e dados pequenos/fixos; `text` para construção dinâmica (ex: `t = text("")` e `t.append(...)`).
3.  **Delimitadores de bloco**: `end if`, `end while`, `end function` são claros, mas verbosos. É a escolha certa para manter a legibilidade do BASIC.
4.  **Sintaxe de comentários**: `//` (como no exemplo) é moderno e amplamente reconhecido. Alternativa clássica seria `REM`.
5.  **Métodos vs. Funções Globais**: Para `list`, `dict`, `stack`, seguir o estilo Python (`.append(valor)`) é mais intuitivo que funções globais (`append(lista, valor)`).

---

arataca89@gmail.com
