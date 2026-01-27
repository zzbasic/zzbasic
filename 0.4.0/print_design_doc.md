## 📘 **DOCUMENTO COMPLETO DE DESIGN - PRINT v0.4.0**

# ZZBASIC PRINT COMMAND - DESIGN SPECIFICATION v0.4.0

## 1. VISÃO GERAL
O comando `print` no ZzBasic v0.4.0 fornece saída formatada com cores, alinhamento e controle de layout, combinando a simplicidade do BASIC tradicional com recursos modernos.

## 2. SINTAXE FORMAL (EBNF)

```
# ------------ PRINT STATEMENT ----------------------------
print_stmt      := ('print' | '?') print_item* nl?
print_item      := expression  
nl              := 'nl'        # New line - quando presente, quebra linha

print_command ::= ('print' | '?') print_item* [';']

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

#--------------------- PRINT STATMENT FIM ----------------------------------
```

## 3. SEMÂNTICA E COMPORTAMENTO

### 3.1 Separadores e Controle de Linha
- **Comportamento padrão**: não quebra a linha
- **`nl` no final**: quebra de linha ('\n')
- **Separador padrão**: Espaço simples entre items
- **`print` vazio**: Imprime uma linha em branco (apenas `\n`)

### 3.2 Ordem de Processamento
Para cada item na sequência:
1. Se for **color_directive**: Ativa/desativa cor
2. Se for **format_directive**: Configura formatação para próximo item
3. Se for **expression**: 
   - Avalia expressão
   - Aplica formatação configurada (width/alignment)
   - Aplica cor ativa
   - Adiciona ao buffer de saída
   - Reseta formatação (não reseta cor)

### 3.3 Regras de Formatação
- **`width(n)`**: Define campo de `n` caracteres para o **PRÓXIMO** item
- **Alinhamentos**: Aplicam-se ao **PRÓXIMO ITEM** 
- **Cores**: Aplicam-se a todos os items subsequentes até `nocolor`

## 4. EXEMPLOS COMPREENSIVOS

### 4.1 Básico
```basic
>> print "Olá Mundo"
Olá Mundo

>> print 42
42

>> let nome = "Zurg"
>> print "Nome:" nome
Nome: Zurg
```

### 4.2 Controle de Linha
```basic
>> print "Linha 1"
>> print " continua"
Linha 1 continua

>> print  # Linha em branco
(linha vazia)

>> print "a" "b" "c"
a b c
```

### 4.3 Expressões
```basic
>> print "Soma:" 10 + 20
Soma: 30

>> let a = 5
>> let b = 3
>> print a "+" b "=" a + b
5 + 3 = 8

>> print "PI =" 3.14159
PI = 3.14159
```

### 4.4 Width (Largura de Campo)
```basic
# width() aplica ao PRÓXIMO item
>> print "Nome" width(15) "Idade" width(10) "Cidade"
Nome            Idade     Cidade

>> print "João" width(15) 25 width(10) "SP"
João            25        SP

# width() persiste até próximo item
>> print width(8) "A" "B" "C"
       A       B       C
```

### 4.5 Alinhamento
```basic
# Alinhamento aplica ao item POSTERIOR
>> print width(5) left "ID" width(8) left "Produto" width(15) "Preço"
ID      Produto         Preço

>> print width(5) left 101 width(8) left "Monitor" width(15) 299.90
101     Monitor         299.90

# Combinações
>> print  center width(20) "Centralizado"
       Texto       

>> print 42 right width(10)
        42
```

### 4.6 Sistema de Cores
```basic
# Cores básicas
>> print red "ERRO" nocolor ": Arquivo não encontrado"
ERRO: Arquivo não encontrado  # "ERRO" em vermelho

>> print green "SUCESSO" nocolor blue " INFO" nocolor
SUCESSO INFO  # "SUCESSO" verde, "INFO" azul

# Cores bright
>> print bred "ALERTA!" nocolor " Mensagem importante"
ALERTA! Mensagem importante  # Bright red

# Cores com expressões
>> let valor = 100
>> print green "Saldo:" nocolor " R$" bred valor nocolor
Saldo: R$ 100  # "Saldo:" verde, 100 bright red

# Cores persistentes
>> print cyan
>> print "Texto 1 em ciano"
>> print "Texto 2 ainda em ciano"
>> nocolor
>> print "Texto 3 normal"
```

### 4.7 Combinações Complexas
```basic
# Cabeçalho colorido
>> print cyan "=== RELATÓRIO DE VENDAS ===" nocolor
>> print

# Tabela com cores
>> print yellow "CÓDIGO" nocolor width(10) 
>> print yellow "PRODUTO" nocolor width(20) 
>> print yellow "ESTOQUE" nocolor
>> print "---" width(10) "-------" width(20) "-------"
>> print "101" width(10) "Monitor" width(20) green "10" nocolor
>> print "102" width(10) "Teclado" width(20) yellow "5" nocolor  
>> print "103" width(10) "Mouse" width(20) red "0" nocolor

# Barra de progresso
>> print "Progresso: [" green;
>> for i = 1 to 10  # Futuro: loop
>>     print "█";
>> print "]" nocolor " 100%"
Progresso: [██████████] 100%
```

## 5. ESPECIFICAÇÃO TÉCNICA

### 5.1 Tokens Novos
```c
// Comando
PRINT, QUESTION_MARK

// Formatação  
WIDTH, LEFT, RIGHT, CENTER

// Cores normais
BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE

// Cores bright
BRED, BGREEN, BYELLOW, BBLUE, BMAGENTA, BCYAN, BWHITE

// Background (futuro)
BGBLACK, BGRED, BGGREEN, BGYELLOW, BGBLUE, BGMAGENTA, BGCYAN, BGWHITE

// Reset
NOCOLOR
```

### 5.2 Estruturas de Dados (ideia inicial, podem ser alteradas no decorrer da implementação)
```c
typedef struct {
    char* text;
    size_t length;
    size_t capacity;
} PrintBuffer;

typedef struct {
    PrintBuffer buffer;
    
    // Estado atual
    int current_color;      // Código ANSI atual
    int field_width;        // Largura para próximo item (0 = sem width)
    char field_align;       // 'L', 'R', 'C', '\0' para próximo item
    int newline;            // 1 - true; 0 - false
    
    // Configuração
    int colors_enabled;    // Terminal suporta cores?
} PrintContext;

typedef struct {
    const char* name;
    const char* ansi_code;
} ColorMapping;
```

### 5.3 Mapeamento ANSI
```c
// Tabela de cores
static ColorMapping color_table[] = {
    // Normais
    {"black",    "\033[30m"},
    {"red",      "\033[31m"},
    {"green",    "\033[32m"},
    {"yellow",   "\033[33m"},
    {"blue",     "\033[34m"},
    {"magenta",  "\033[35m"},
    {"cyan",     "\033[36m"},
    {"white",    "\033[37m"},
    
    // Bright
    {"bred",     "\033[91m"},
    {"bgreen",   "\033[92m"},
    {"byellow",  "\033[93m"},
    {"bblue",    "\033[94m"},
    {"bmagenta", "\033[95m"},
    {"bcyan",    "\033[96m"},
    {"bwhite",   "\033[97m"},
    
    // Background (futuro)
    {"bgblack",  "\033[40m"},
    {"bgred",    "\033[41m"},
    {"bggreen",  "\033[42m"},
    {"bgyellow", "\033[43m"},
    {"bgblue",   "\033[44m"},
    {"bgmagenta","\033[45m"},
    {"bgcyan",   "\033[46m"},
    {"bgwhite",  "\033[47m"},
    
    // Reset
    {"nocolor",  "\033[0m"},
    {NULL, NULL}
};
```

### 5.4 Algoritmo Principal
```
PROCEDURE ExecutePrint(tokens, symbol_table)
    context = CreatePrintContext()
    
    WHILE has_more_tokens(tokens)
        token = next_token(tokens)
        
        CASE token.type OF
            COLOR_TOKEN:
                ApplyColor(context, token)
                
            WIDTH_TOKEN:
                context.field_width = parse_width_argument()
                
            ALIGNMENT_TOKEN:
                context.field_align = token_to_alignment(token)
                
            EXPRESSION:
                value = EvaluateExpression(token, symbol_table)
                str = ValueToString(value)
                
                IF context.field_align != '\0'
                    str = ApplyAlignment(str, context.field_align)
                    context.field_align = '\0'
                
                IF context.field_width > 0
                    str = ApplyWidth(str, context.field_width)
                    context.field_width = 0
                
                IF context.current_color != DEFAULT_COLOR
                    AppendColorCode(context, context.current_color)
                
                AppendString(context, str)
                
            NL:
                context.newline = TRUE
                
        END CASE
    END WHILE
    
    IF NOT context.suppress_newline
        AppendNewline(context)
    
    OutputToTerminal(context.buffer)
    DestroyPrintContext(context)
END PROCEDURE
```

## 6. CASOS ESPECIAIS E TRATAMENTO DE ERROS

### 6.1 Validações
```basic
# Erro: width() sem argumento
>> print width()
Error: width() expects a number argument

# Erro: width() com argumento inválido
>> print width("texto")
Error: width() argument must be a number

# Aviso: Terminal não suporta cores
>> print red "texto"  # Em terminal sem cores
texto  # Sem código ANSI
```

### 6.2 Comportamentos de Borda
```basic
# width() seguido de múltiplos items
>> print width(5) "a" "b" "c"
    a    b    c  # Cada item tem width(5)

# Alinhamento sem width
>> print "texto" left  # Ignorado (sem efeito)
texto

# Cores aninhadas
>> print red "vermelho" blue "azul" nocolor
# "vermelho" em red, "azul" em blue

# Reset parcial
>> print red bgblue "texto" nocolor
# Remove ambas as cores
```

## 7. PLANO DE IMPLEMENTAÇÃO

### Fase 1: Núcleo 
1. Token `PRINT` e `?`
2. Múltiplas expressões com espaço
3. Controle de nova linha com `;`
4. Conversão automática de tipos

### Fase 2: Cores ANSI 
1. Sistema básico de cores (8 normais + nocolor)
2. Cores bright (8)
3. Detecção automática de suporte a cores

### Fase 3: Formatação 
1. `width()` básico
2. Alinhamento `left/right/center`
3. Combinações de formatação

### Fase 4: Polimento 
1. Background colors (opcional)
2. Otimizações de performance
3. Testes abrangentes
4. Documentação final

## 8. CONSIDERAÇÕES DE PERFORMANCE

### 8.1 Bufferização
- Buffer dinâmico para evitar realocações frequentes
- Tamanho inicial: 256 bytes, cresce conforme necessário

### 8.2 Códigos ANSI
- Apenas inserir códigos ANSI se terminal suportar
- Variável global `COLORS_ENABLED` detectada no startup

### 8.3 Cache de Conversão
- Cache simples para conversão número→string
- LRU cache com 16 entradas para números frequentes

## 9. TESTES RECOMENDADOS

### Unitários:

### Integração:


## 10. NOTAS DE COMPATIBILIDADE

### 10.1 vs BASIC Tradicional
- `?` como atalho mantido
- `nl` para quebra de linha
- `width()` substitui `TAB()` mais flexível
- Cores ANSI adicionadas (novo)

### 10.2 vs Python
- Sem parênteses obrigatórios
- `sep` fixo como espaço (não configurável)
- `end=''` não possui; basta colocar os itens sem separação: `print "TudoJunto"`
- Sistema de cores integrado

### 10.3 Cross-platform
- Cores ANSI funcionam em Linux/macOS/Windows 10+
- Fallback automático para sem cores
- `isatty()` para detectar terminal interativo

## 11. CONCLUSÃO

O comando `print` do ZzBasic v0.4.0 oferece:
1. **Simplicidade** do BASIC clássico
2. **Poder** de formatação moderna
3. **Beleza** das cores ANSI
4. **Flexibilidade** para diversos casos de uso

---

arataca89@gmail.com
