# TESTE

---

```
# ============================================================
# ZzBasic v0.4.2 - Script de Teste Completo
# Testa todas as funcionalidades implementadas até agora
# ============================================================

# Título
print nl
print blue "+--------------------------------------------------------+" nl
print blue "|                  "cyan"ZzBasic v0.4.2                      "blue"|" nl
print blue "+--------------------------------------------------------+" nl
print nl

# ============================================================
# 1. Expressões Aritméticas (v0.1.0)
# ============================================================
nocolor
print "=======================================================" nl
print yellow "1. EXPRESSÕES ARITMÉTICAS" nl
nocolor
print "=======================================================" nl
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

print "=======================================================" nl
print yellow "2. VARIÁVEIS E COMANDO LET" nl
nocolor
print "=======================================================" nl
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
let versao = "0.4.2"
print "Linguagem: " nome " " versao nl
print nl

# ============================================================
# 3. Comando PRINT (v0.4.0)
# ============================================================

print "=======================================================" nl
print yellow "3. COMANDO PRINT" nl
nocolor
print "=======================================================" nl
print nl

print "Esta linha não pula para a próxima."
print " Esta continua na mesma linha." nl
print "Esta linha pula para a próxima." nl
print nl

# ============================================================
# 4. Cores (v0.4.1)
# ============================================================

print "=======================================================" nl
print yellow "4. CORES" nl
nocolor
print "=======================================================" nl
print nl

print red "ERRO:" nocolor " Arquivo não encontrado." nl
print green "SUCESSO:" nocolor " Operação concluída." nl
print yellow "AVISO:" nocolor " Verifique os dados." nl
print blue "INFO:" nocolor " Leia com atenção." nl
print nl

# ============================================================
# 5. Formatação (v0.4.2)
# ============================================================

print "=======================================================" nl
print yellow "5. FORMATAÇÃO (WIDTH E ALINHAMENTO)" nl
nocolor
print "=======================================================" nl
print nl

# Tabela de Produtos
print cyan width(20) left "Produto" nocolor " | " cyan width(10) right "Preço" nocolor " | " cyan width(10) center "Estoque" nocolor nl
print cyan "──────────────────────────────────────────" nocolor nl
print width(20) left "Notebook" " | " width(10) right 4500.50 " | " width(10) center 15 nl
print width(20) left "Mouse" " | " width(10) right 89.90 " | " width(10) center 120 nl
print width(20) left "Teclado Mecânico" " | " width(10) right 350.00 " | " width(10) center 42 nl
print nl

# Combinação de Cores e Formatação
print red width(40) center "ALERTA DE SEGURANÇA" nocolor nl
print yellow width(40) center "Verifique as permissões do sistema" nocolor nl
print nl

# ============================================================
# 6. Testes de Erro
# ============================================================

print "=======================================================" nl
print yellow "6. TESTES DE ERRO" nl
nocolor
print "=======================================================" nl
print nl

print 10 / 0 nl  # Divisão por zero
#print 5 % 6 nl   # Operador inválido
#let x = 10 +     # Expressão incompleta
#print width(abc) "teste" nl # Argumento inválido para width

print green "Todos os testes (exceto os de erro) foram concluídos com sucesso!" nocolor nl
print nl

# Finalização
print nl
print blue "+--------------------------------------------------------+" nl
print blue "|                  "cyan"FIM DO TESTE                        "blue"|" nl
print blue "+--------------------------------------------------------+" nl
print nl

nocolor
``´
---
arataca89@gmail.com
Aulas particulares de programação

