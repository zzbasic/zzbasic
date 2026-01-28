# ZzBasic - Documentação v0.4.0

![ZzBasic](zzbasic_logo1.png)

**ZzBasic** é uma linguagem de programação interpretada que combina a simplicidade do BASIC clássico com recursos modernos. É um projeto solo desenvolvido com cuidado e atenção aos detalhes.

## Versão Atual: **0.4.0** 🚧

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

### 📋 Histórico de Versões
| Versão | Status | Recursos Principais |
|--------|--------|-------------------|
| 0.1.0 | Lançada | Expressões matemáticas básicas |
| 0.2.0 | Lançada | Variáveis (`let`), strings, tabela de símbolos |
| 0.3.0 | Lançada | Múltiplos statements, REPL avançado, comandos de debug |
| **0.4.0** | Lançada | Comando `print`, controle de linha, saída formatada |

---

## 📥 Instalação e Compilação

### Pré-requisitos
- **Compilador C**: GCC, Clang ou MSVC
- **Make** (opcional)
- **Git** (para clonar)

### Compilação (Linux/macOS)
```bash
# Clone o repositório
git clone [repositório]
cd zzbasic

# Compile
make

# Execute
./zzbasic
```

### Compilação (Windows)
```bash
# Com MinGW
gcc -o zzbasic *.c
zzbasic.exe
```

### Testando
```bash
# REPL
$ ./zzbasic
ZzBasic v0.4.0 on Linux
> 5 + 3 * 2
11
> exit

# Arquivo
$ echo 'let x = 10 : let y = 20 : x + y' > teste.zz
$ ./zzbasic teste.zz
30
```

---

## 🎯 Tutorial Rápido v0.4.0

### 1. NOVO: Comando `print` 🖨️

#### Básico:
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

### 2. 📖 Filosofia do `print` no ZzBasic

O ZzBasic adota uma abordagem **simplificada e consistente** para o comando `print`:

#### 🎯 **Princípios de Design:**
- **Um comando, uma ação**: Cada `print` é uma ação completa
- **Controle explícito**: Use `nl` quando quiser quebrar linha
- **Sem ambiguidades**: Sem `;` ou `:` após `print`
- **Expressões inline**: Avaliação automática de expressões

#### ✅ **O que TEMOS:**
```basic
# Um print, múltiplos itens (CORRETO)
print "Nome:" nome ", Idade:" idade nl

# Controle explícito de linha
print "Texto"     # Fica na mesma linha (padrão)
print "outro" nl  # Quebra linha após imprimir

# Expressões matemáticas
print "Soma:" 10 + 20 "=" 30 nl
```

#### 🚫 **O que NÃO TEMOS (e por quê):**
```basic
# ❌ NÃO FAÇA: Dois prints sem separador
print "A" print "B"      # ERRO: print não é expressão válida

# ❌ NÃO FAÇA: ; após print  
print "A"; print "B"     # ERRO: print não precisa de ;

# ❌ NÃO FAÇA: : após print
print "A": print "B"     # ERRO: use nl (nova linha)
```

#### 🧠 **Por que esta abordagem?**
> "Um comando deve fazer uma coisa bem. Se precisa imprimir múltiplos valores, 
> faça em um único `print`. Se precisa controlar linhas, use `nl` explicitamente.
> Simplicidade acima de flexibilidade desnecessária."

#### 📚 **Comparação com outras linguagens:**
| Linguagem | Sintaxe | Filosofia |
|-----------|---------|-----------|
| BASIC tradicional | `PRINT A; B;` | Concatenar com controle implícito |
| Python | `print(a, b, end=" ")` | Flexibilidade com parâmetros |
| **ZzBasic** | `print a b nl` | **Simplicidade explícita** |

### 3. 📝 Exemplos Pedagógicos Graduais

```basic
# Nível 1: Básico
print "Olá"           # Imprime e fica na linha
print "Mundo" nl      # Imprime e quebra linha

# Nível 2: Múltiplos itens  
print "Nome:" nome nl # Variável após texto

# Nível 3: Expressões
print "Dobro:" n * 2 nl  # Cálculo inline

# Nível 4: Construção complexa
print "Progresso: ["
for i = 1 to 10        # Futuro: loops
    print "."
print "]" nl
```

### 4. 🚨 Mensagens de Erro Específicas

O ZzBasic dá **feedback claro** sobre erros comuns:

```basic
>> print "A"; print "B"
Error [1:10]: print statement cannot have ';' after it. 
              Remove the ';' or use: print "text1" "text2" nl

>> print "A": print "B"
Error [1:10]: print statement cannot have ':' after it. 
              Use new line for next statement.

>> print print "teste"
Error [1:7]: Unexpected 'print' in expression
```

### 5. 🎮 REPL Avançado (atualizado)

```basic
# Testando o novo print no REPL
>> tokens print "Olá" nome nl
=== LEXICAL ANALYSIS ===
  1: (1:1)[PRINT]: print
  2: (1:7)[STRING]: "Olá"
  3: (1:12)[IDENTIFIER]: nome
  4: (1:17)[NL]: nl
  5: (1:19)[EOF]
=== END OF ANALYSIS ===

>> ast print "Olá" nome nl
AST for: "print "Olá" nome nl"
[1:1] STATEMENT_LIST (1 statements)
    [1:1] PRINT (2 items) [newline]
        [1:7] STRING: "Olá"
        [1:12] VARIABLE: nome
```

---

## 📚 Referência da Linguagem v0.4.0

### Gramática:
```ebnf
# =================================================
# ZzBasic - GRAMÁTICA v0.4.0 - PRINT
# =================================================

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
             | background_color  

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

```

### Comportamento do `print`:
| Código | Resultado | Comportamento |
|--------|-----------|---------------|
| `print "A"` | `A` (sem quebra) | Acumula na linha |
| `print "A" nl` | `A\n` | Quebra após imprimir |
| `print "A" "B"` | `AB` (sem quebra) | Acumula ambos |
| `print "A" "B" nl` | `AB\n` | Quebra no final |
| `print nl` | `\n` | Linha em branco |

### Regras do `print`:
1. **Único comando**: Apenas um `print` por linha (até `nl`/`EOL`/`EOF`)
2. **Múltiplos itens**: Separe por espaço: `print item1 item2 item3 nl`
3. **Controle explícito**: Sem `nl` = mesma linha, com `nl` = quebra linha
4. **Sem separadores**: Não use `;` ou `:` após `print`
5. **Expressões válidas**: Números, strings, variáveis, operações matemáticas

### Novos Tokens (v0.4.0):
```c
TOKEN_PRINT     // print
TOKEN_QUESTION  // ? (atalho)
TOKEN_NL        // nl (new line control)
```

---

## 💡 Exemplos Práticos v0.4.0

### 1. Calculadora com Saída Formatada:
```basic
# calculadora.zz
let a = 10
let b = 20

print "Calculadora ZzBasic" nl
print "===================" nl
print "A =" a nl
print "B =" b nl
print "Soma:" a "+" b "=" a + b nl
print "Produto:" a "*" b "=" a * b nl
```

### 2. Relatório Simples:
```basic
# relatorio.zz
let produto = "Monitor LED"
let preco = 299.90
let quantidade = 3
let total = preco * quantidade

print "RELATÓRIO DE VENDA" nl
print "=================" nl
print "Produto:" produto nl
print "Preço unitário: R$" preco nl
print "Quantidade:" quantidade nl
print "Total: R$" total nl
```

### 3. Interface Interativa:
```basic
# Bem-vindo ao REPL
>> print "=== SISTEMA ZzBasic v0.4.0 ===" nl
>> print "Digite 'help' para ajuda" nl
>> print "ou 'exit' para sair" nl
>> 
```

---

## 🔮 Roadmap 

### Versão 0.4.0 - PRINT e Saída **(CONCLUÍDA)**
- [✅] Comando `print` básico
- [✅] Atalho `?`
- [✅] Controle de linha com `nl`
- [✅] Implementação do evaluator para `print`
- [✅] Integração com REPL
- [✅] Testes 

### Versão 0.4.1 - Cores e Formatação **(PRÓXIMO)**
- [ ] Cores ANSI (16 cores básicas + bright)
- [ ] Comando `width()` para largura fixa
- [ ] Alinhamento `left`, `right`, `center`
- [ ] Comando `nocolor` para reset

---

## 🛠️ Solução de Problemas v0.4.0

### Erros Comuns com `print`:

#### "Unexpected ';' in expression"
```basic
>> print "A"; print "B"
Error: print statement cannot have ';' after it.
```
**Solução:** Use um único `print` com múltiplos itens:
```basic
print "A" "B" nl
```

#### "Unexpected ':' in expression"
```basic
>> print "A": print "B"
Error: print statement cannot have ':' after it.
```
**Solução:** Use nova linha para separar statements:
```basic
print "A" nl
print "B" nl
```

#### "Unexpected 'print' in expression"
```basic
>> print print "texto"
Error: 'print' is not a valid expression.
```
**Solução:** Apenas um `print` por comando:
```basic
print "texto" nl
```

#### Print não quebra linha
```basic
>> print "texto"
# Nada acontece? Aguarde próximo comando...
>> print " continua" nl
texto continua
```
**Solução:** Use `nl` para quebrar linha explicitamente, ou o próximo `print` continuará na mesma linha.

---

## 📝 Sobre o Desenvolvimento v0.4.0

### Novidades Técnicas:
1. **Nó AST para `print`** - Estrutura otimizada para múltiplos itens
2. **Parser específico** - Tratamento especial para comandos de saída
3. **Mensagens de erro contextualizadas** - Ajuda clara para usuários
4. **Design consistente** - Filosofia "simples e explícito" aplicada ao `print`

### Filosofia Aplicada:
O `print` do ZzBasic reflete nossa filosofia central:
- **Explícito sobre implícito** (`nl` vs comportamento automático)
- **Simples sobre complexo** (um comando, múltiplos itens)
- **Consistente sobre flexível** (regras claras, sem exceções)

### Para Desenvolvedores:
- **Código modular** - Parser separado para cada tipo de statement
- **Mensagens úteis** - Erros que ensinam a usar corretamente
- **Preparado para extensão** - Estrutura pronta para cores e formatação

---

arataca89@gmail.com

Aulas particulares de programação


