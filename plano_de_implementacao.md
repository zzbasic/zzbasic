# 🗺️ ZzBasic - Plano de Implementação Técnica (Incremental)

## 📁 Estrutura do Projeto (Desde o Início)
Sugiro criar esta estrutura de pastas no repositório `zzbasic/zzbasic` para manter a organização profissional desde o início:
```
zzbasic/                   # Raiz do repositório
├── sources.txt            # Lista de arquivos para a a89alloc compilar
├── zzbasic.h              # Cabeçalho principal
├── main.c                 # Ponto de entrada, REPL
├── lexer.c                # Análise léxica
├── parser.c               # Análise sintática
├── ast.c                  # Definição e funções da AST
├── evaluator.c            # Avaliação
├── builtins.c             # print, input, cores
├── environment.c          # Tabela de símbolos
├── a89alloc.c             # SUA biblioteca de alocação
├── a89alloc.h             # Header da sua biblioteca
├── utils.c                # (Opcional) Funções auxiliares gerais
└── examples/              # Pasta para scripts .zz
    └── hello.zz
```

## 🔤 Convenções e Decisões Técnicas Iniciais
*   **Extensão de Arquivo:** `.zz` é **perfeito**. Curta, única e óbvia.
*   **Comando REPL:** `zzbasic` (sem argumentos) inicia o REPL. `zzbasic script.zz` executa o arquivo.
*   **Gerenciamento de Memória:** Comece com alocação simples (`malloc/free`). Pode evoluir para um *garbage collector* básico nas versões com `list` e `dict`.

## 🚀 Roadmap Incremental Detalhada

### **Versão 0.0.0 - Esqueleto e REPL Básico**
**Objetivo:** Ter um programa que compila, roda e lê entrada.
*   [✅] Criar `main()` que detecta se é REPL ou execução de arquivo.
*   [✅] Implementar loop REPL básico: exibe prompt `>> `, lê linha, ecoa de volta.
*   [✅ ] Compilar com e criar executável `zzbasic`.

### **Versão 0.1.0 - Avaliador de Expressões Aritméticas**
**Objetivo:** Calcular `2 + 3 * (4 - 1)`.
*   [✅ ] **Lexer:** Tokenizar números, operadores `+ - * / %` e parênteses.
*   [ ] **Parser:** Construir uma Árvore de Sintaxe Abstrata (AST) para uma expressão.
*   [ ] **Eval:** Percorrer a AST e calcular o resultado.
*   [ ] No REPL: digitar `>> 5 + 3` exibe `8`.

### **Versão 0.2.0 - Variáveis e Comando `LET`**
**Objetivo:** Atribuir e recuperar valores: `LET x = 10`.
*   [ ] Adicionar token `LET` e identificadores (nomes de variáveis) ao lexer.
*   [ ] Estender o parser para a forma `LET <id> = <expr>`.
*   [ ] Criar uma tabela de símbolos simples (hashmap ou array) para armazenar pares `nome -> valor`.
*   [ ] No REPL: `>> LET pi = 3.14` depois `>> pi * 2` exibe `6.28`.

### **Versão 0.3.0 - Múltiplas Statements (Declarações)**
**Objetivo:** Executar mais de um comando por linha/arquivo.
*   [ ] Modificar o parser para lidar com uma lista de statements separados por nova linha ou `:`.
*   [ ] Criar loop no eval para executar cada statement em sequência.
*   [ ] Exemplo de script `.zz`: `LET a = 5\nLET b = a * 2\nprint a`.

### **Versão 0.4.0 - Comando `PRINT`**
**Objetivo:** Exibir resultados e texto.
*   [ ] Adicionar token `PRINT`.
*   [ ] Parser para `PRINT <expr1>, <expr2>, ...`.
*   [ ] Implementar função `builtin_print()` que formata valores (números como strings) e os exibe.
*   [ ] Exemplo: `PRINT "O resultado é: ", 10 + 2`.

### **Versão 0.5.0 - Comando `INPUT`**
**Objetivo:** Ler dados do usuário.
*   [ ] Token `INPUT` e parser para `INPUT "prompt: ", <var>`.
*   [ ] Função `builtin_input()` que lê uma linha do stdin e converte para `number` ou `string`.
*   [ ] Armazenar o valor lido na variável especificada.

### **Versões 0.6.0 a 0.8.0 - Controle de Fluxo**
Seguir a mesma lógica para cada comando:
1.  **`IF` (0.6.0):** Adicionar tokens `IF`, `THEN`, `ELSE`, `END IF`. Parser para condicionais. Avaliar a condição e pular para o bloco correto.
2.  **`FOR` (0.7.0):** Tokens `FOR`, `TO`, `STEP`, `NEXT`. Implementar loop com variável de controle.
3.  **`WHILE` (0.8.0):** Tokens `WHILE`, `END WHILE`. Loop baseado em condição.

### **Versão 0.9.0 - Funções e Escopo**
**Objetivo:** `FUNCTION soma(a, b) ... END FUNCTION`.
*   [ ] Tokens `FUNCTION`, `RETURN`, `END FUNCTION`.
*   [ ] Parser para definição de função (corpo como bloco de statements).
*   [ ] Tabela de símbolos com suporte a escopos aninhados (pilha de ambientes).
*   [ ] Avaliação de chamada de função: `resultado = soma(5, 3)`.

### **Versão 0.10.0 - `STRUCT`**
**Objetivo:** Criar tipos compostos.
*   [ ] Tokens `STRUCT`, `END STRUCT`.
*   [ ] Parser para definir uma struct com uma lista de membros.
*   [ ] Implementar struct como um `dict` especial no runtime, onde as chaves são os nomes dos membros.
*   [ ] Acesso com operador `.`: `pessoa.nome = "João"`.

### **Versão 0.11.0 - Módulos (Uma Abordagem Incremental Simples)**
**Objetivo Básico:** `IMPORT "matematica"` para usar funções de outro arquivo `.zz`.
*   [ ] Token `IMPORT`.
*   [ ] Mecanismo simples: ao encontrar `IMPORT "outro.zz"`, o interpretador **executa** o arquivo importado em um **novo escopo isolado**.
*   [ ] Após a execução, todas as variáveis/funções **globais** definidas naquele escopo são **copiadas** para o escopo atual.
*   [ ] **Isso é um início funcional e compreensível.** Mais tarde pode-se evoluir para um sistema de cache de módulos e caminhos de busca.

## 💡 Dicas para Cada Etapa

1.  **Comece pelo Lexer e Parser Simples:** Não tente fazer um parser para a linguagem completa de uma vez. Faça para **uma única funcionalidade por vez** (ex: só expressões, depois só `LET`).
2.  **Use `union` e `enum` em C para a AST:** Isso tornará o código do eval muito mais limpo.
3.  **Escreva MUITOS testes:** Para cada versão, crie um arquivo `testes_v0.1.0.zz` e verifique se a saída é a esperada.
4.  **Comprometa-se com as versões:** A beleza do plano incremental é que **cada versão é uma vitória**. A versão 0.0.0 já é um produto funcional (o REPL).


**Próximo passo concreto:** Começar a implementar a **estrutura do projeto (Makefile, `src/`)** e a **Versão 0.0.0**. Posso te ajudar com um esboço do código C inicial se desejar.
