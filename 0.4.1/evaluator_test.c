// evaluator_test.c - Testes específicos para o evaluator com cores

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "evaluator.h"

// ============================================
// FUNÇÕES DE TESTE AUXILIARES
// ============================================

// Executa um código ZzBasic e verifica o resultado
void run_test(const char* test_name, const char* code, int expect_success) {
    printf("\n%s%s%s\n", COLOR_HEADER, test_name, COLOR_RESET);
    printf("Código: %s\n", code);
    
    SymbolTable* symbols = symbol_table_create();
    Lexer lexer;
    lexer_init(&lexer, code);
    
    ASTNode* ast = parse(&lexer);
    if (!ast) {
        printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        symbol_table_destroy(symbols);
        return;
    }
    
    printf("AST:\n");
    print_ast(ast, 1);
    
    int success = evaluate_program(ast, symbols);
    
    if (success == expect_success) {
        printf("%s✓ Teste passou%s\n", COLOR_SUCCESS, COLOR_RESET);
    } else {
        printf("%s✗ Teste falhou%s\n", COLOR_ERROR, COLOR_RESET);
        printf("  Esperado: %s\n", expect_success ? "sucesso" : "erro");
        printf("  Obtido: %s\n", success ? "sucesso" : "erro");
    }
    
    free_ast(ast);
    symbol_table_destroy(symbols);
    wait();
}

// Executa uma sequência de comandos
void run_sequence(const char* test_name, const char** commands) {
    printf("\n%s%s%s\n", COLOR_HEADER, test_name, COLOR_RESET);
    
    SymbolTable* symbols = symbol_table_create();
    
    for (int i = 0; commands[i] != NULL; i++) {
        printf("  > %s\n", commands[i]);
        
        Lexer lexer;
        lexer_init(&lexer, commands[i]);
        
        ASTNode* ast = parse(&lexer);
        if (ast) {
            evaluate_program(ast, symbols);
            free_ast(ast);
        } else {
            printf("%s✗ Erro no parsing%s\n", COLOR_ERROR, COLOR_RESET);
        }
    }
    
    symbol_table_destroy(symbols);
    printf("%s✓ Sequência completa%s\n", COLOR_SUCCESS, COLOR_RESET);
    wait();
}

// ============================================
// TESTES ESPECÍFICOS DE CORES
// ============================================

void test_colors_basic(void) {
    printf("%s=== TESTES BÁSICOS DE CORES ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Teste 1: Cores normais
    run_test("1.1 - Vermelho básico", 
             "print red \"ERRO\" nocolor \": Arquivo não encontrado\" nl", 1);
    
    run_test("1.2 - Verde e azul", 
             "print green \"SUCESSO\" nocolor blue \" INFO\" nocolor nl", 1);
    
    // Teste 2: Cores bright
    run_test("2.1 - Bright red", 
             "print bred \"ALERTA!\" nocolor \" Mensagem importante\" nl", 1);
    
    run_test("2.2 - Bright cyan", 
             "print bcyan \"NOTIFICAÇÃO\" nocolor nl", 1);
    
    // Teste 3: Todas as cores básicas
    printf("\n%s3. Demonstração todas cores básicas:%s\n", COLOR_INFO, COLOR_RESET);
    const char* cores_demo[] = {
        "print black \" Preto \" nocolor",
        "print red \" Vermelho \" nocolor",
        "print green \" Verde \" nocolor", 
        "print yellow \" Amarelo \" nocolor",
        "print blue \" Azul \" nocolor",
        "print magenta \" Magenta \" nocolor",
        "print cyan \" Ciano \" nocolor",
        "print white \" Branco \" nocolor nl",
        NULL
    };
    run_sequence("Cores normais", cores_demo);
    
    // Teste 4: Cores bright
    printf("\n%s4. Demonstração cores bright:%s\n", COLOR_INFO, COLOR_RESET);
    const char* bright_demo[] = {
        "print bred \" Bright Red \" nocolor",
        "print bgreen \" Bright Green \" nocolor",
        "print byellow \" Bright Yellow \" nocolor",
        "print bblue \" Bright Blue \" nocolor",
        "print bmagenta \" Bright Magenta \" nocolor",
        "print bcyan \" Bright Cyan \" nocolor",
        "print bwhite \" Bright White \" nocolor nl",
        NULL
    };
    run_sequence("Cores bright", bright_demo);
}

void test_colors_with_expressions(void) {
    printf("\n%s=== CORES COM EXPRESSÕES ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Teste com variáveis
    run_sequence("5.1 - Cores com variáveis", (const char*[]){
        "let saldo = 100",
        "print green \"Saldo:\" nocolor \" R$\" bred saldo nocolor nl",
        NULL
    });
    
    // Teste com cálculos
    run_sequence("5.2 - Cores com cálculos", (const char*[]){
        "let preco = 10.5",
        "let quantidade = 3",
        "let total = preco * quantidade",
        "print blue \"Preço:\" nocolor \" \" preco nl",
        "print blue \"Quantidade:\" nocolor \" \" quantidade nl",
        "print green \"Total:\" nocolor \" R$\" yellow total nocolor nl",
        NULL
    });
    
    // Teste com strings
    run_sequence("5.3 - Cores com strings", (const char*[]){
        "let usuario = \"João\"",
        "let acesso = \"admin\"",
        "print cyan \"Usuário:\" nocolor \" \" magenta usuario nocolor nl",
        "print cyan \"Tipo:\" nocolor \" \" green acesso nocolor nl",
        NULL
    });
}

void test_color_persistence(void) {
    printf("\n%s=== PERSISTÊNCIA DE CORES ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Teste 1: Cor persiste entre múltiplos prints
    run_sequence("6.1 - Persistência básica", (const char*[]){
        "print red",
        "print \"Linha 1 vermelha\" nl",
        "print \"Linha 2 ainda vermelha\" nl",
        "print \"Linha 3 continua vermelha\" nl",
        "nocolor",
        "print \"Linha 4 normal\" nl",
        NULL
    });
    
    // Teste 2: Mudança de cor entre prints
    run_sequence("6.2 - Mudança de cores", (const char*[]){
        "print green \"INÍCIO\" nl",
        "print \"Texto ainda verde\" nl",
        "print blue",
        "print \"Agora azul\" nl",
        "print \"Continua azul\" nl",
        "print magenta",
        "print \"Agora magenta\" nl",
        "nocolor",
        "print \"Fim normal\" nl",
        NULL
    });
    
    // Teste 3: Reset explícito
    run_sequence("6.3 - Reset explícito", (const char*[]){
        "print cyan \"Texto ciano\" nl",
        "print \"Ainda ciano\" nl",
        "nocolor",  // Comando explícito
        "print \"Agora normal\" nl",
        "print yellow \"Amarelo\" nl",
        "print \"Ainda amarelo?\" nl",  // Deve ser normal (nocolor resetou)
        NULL
    });
}

void test_color_errors(void) {
    printf("\n%s=== TESTES DE ERRO (cores) ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Teste 1: Cor inexistente (não deve acontecer - lexer já bloqueia)
     run_test("7.1 - Cor inválida", "print roxo \"texto\" nl", 0);
    
    // Teste 2: Cor no lugar errado (parser já trata)
     run_test("7.2 - Cor em expressão matemática", "let x = red + 5", 0);
    
    // Teste 3: Print vazio com cor
    run_test("7.3 - Print vazio com cor", "print red nl", 1);
    
    // Teste 4: Apenas nocolor
    run_test("7.4 - Apenas nocolor", "nocolor", 1);
}

void test_color_real_world(void) {
    printf("\n%s=== CENÁRIOS DO MUNDO REAL ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Cenário 1: Sistema de logging
    printf("\n%s1. Sistema de logging:%s\n", COLOR_INFO, COLOR_RESET);
    run_sequence("Logging system", (const char*[]){
        "print green \"[INFO] \" nocolor \"Sistema iniciado\" nl",
        "print yellow \"[WARN] \" nocolor \"Cache quase cheio\" nl",
        "print red \"[ERROR] \" nocolor \"Falha na conexão\" nl",
        "print bcyan \"[DEBUG] \" nocolor \"Variável x = 42\" nl",
        NULL
    });
    
    // Cenário 2: Tabela de dados
    printf("\n%s2. Tabela de produtos:%s\n", COLOR_INFO, COLOR_RESET);
    run_sequence("Tabela de produtos", (const char*[]){
        "print cyan \"=== LISTA DE PRODUTOS ===\" nocolor nl",
        "print yellow \"CÓDIGO\" nocolor \"    \"",
        "print yellow \"PRODUTO\" nocolor \"     \"",
        "print yellow \"ESTOQUE\" nocolor nl",
        "print \"---    \" \"-------     \" \"-------\" nl",
        "print \"101\" \"    \" \"Monitor\" \"     \" green \"10\" nocolor nl",
        "print \"102\" \"    \" \"Teclado\" \"     \" yellow \"5\" nocolor nl", 
        "print \"103\" \"    \" \"Mouse\" \"     \" red \"0\" nocolor nl",
        "print \"104\" \"    \" \"Webcam\" \"     \" green \"15\" nocolor nl",
        NULL
    });
    
    // Cenário 3: Barra de progresso simulada
    printf("\n%s3. Barra de progresso:%s\n", COLOR_INFO, COLOR_RESET);
    run_sequence("Barra de progresso", (const char*[]){
        "print \"Progresso: [\" green",
        "print \"█\"",
        "print \"█\"", 
        "print \"█\"",
        "print \"█\"",
        "print \"█\"",
        "print \"]\" nocolor \" 50%\" nl",
        NULL
    });
    
    // Cenário 4: Menu colorido
    printf("\n%s4. Menu interativo:%s\n", COLOR_INFO, COLOR_RESET);
    run_sequence("Menu colorido", (const char*[]){
        "print bcyan \"=== MENU PRINCIPAL ===\" nocolor nl nl",
        "print green \"1. \" nocolor \"Nova entrada\" nl",
        "print green \"2. \" nocolor \"Listar registros\" nl",
        "print green \"3. \" nocolor \"Exportar dados\" nl",
        "print red \"4. \" nocolor \"Sair\" nl nl",
        "print yellow \"Opção: \" nocolor",
        NULL
    });
}

// ============================================
// TESTES DE COMPATIBILIDADE (v0.3.0)
// ============================================

void test_compatibility(void) {
    printf("%s=== TESTES DE COMPATIBILIDADE ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Testes antigos devem continuar funcionando
    run_test("C1 - Expressão básica", "5 + 3", 1);
    run_test("C2 - Atribuição", "let x = 10", 1);
    run_test("C3 - Print simples", "print \"Hello World\" nl", 1);
    run_test("C4 - Variável no print", "let msg = \"Olá\" : print msg nl", 1);
    run_test("C5 - Múltiplos statements", "let a = 5 : let b = 10 : a + b", 1);
}

// ============================================
// FUNÇÃO PRINCIPAL
// ============================================

int main(void) {
    setup_utf8();
    
    printf("%s┌───────────────────────────────────────┐%s\n", COLOR_HEADER, COLOR_RESET);
    printf("%s│   TESTE EVALUATOR ZzBasic v0.4.1      │%s\n", COLOR_HEADER, COLOR_RESET);
    printf("%s│   Sistema de Cores ANSI               │%s\n", COLOR_HEADER, COLOR_RESET);
    printf("%s└───────────────────────────────────────┘%s\n\n", COLOR_HEADER, COLOR_RESET);
    
    // Reset inicial (garante estado limpo)
    printf("%sEstado inicial: cores resetadas%s\n\n", COLOR_INFO, COLOR_RESET);
    
    // Executa todos os testes
    test_compatibility();
    wait();
    
    test_colors_basic();
    wait();
    
    test_colors_with_expressions();
    wait();
    
    test_color_persistence();
    wait();
    
    test_color_errors();
    wait();
    
    test_color_real_world();
    wait();
    
    // Verificação final
    printf("\n%s=== VERIFICAÇÃO FINAL ===%s\n", COLOR_HEADER, COLOR_RESET);
    
    // Reset final (boa prática)
    printf("%s", COLOR_RESET);
    
#ifdef A89ALLOC_DEBUG
    a89check_leaks();
#endif
    
    printf("\n%s✅ Todos os testes de cores executados com sucesso!%s\n", 
           COLOR_SUCCESS, COLOR_RESET);
    
    return 0;
}