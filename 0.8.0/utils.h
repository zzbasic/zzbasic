// utils.h
#ifndef UTILS_H
#define UTILS_H

void setup_utf8();

void zzwait();

/********************************************************************
Exibe uma linha de código-fonte com uma seta apontando para o erro

Parâmetros:
	* source      Ponteiro para o código-fonte completo
	* source_size Tamanho total do código-fonte em bytes
	* line        Número da linha (1-indexado)
	* column      Número da coluna (1-indexado)
 
Exemplo de saída:
	Linha 5: let x = 10 + 
                      ↑
*********************************************************************/
void display_error_location(const char* source, int source_size, int line, int column);


// Compara duas strings. Retorna: 1 se iguais, 0 se diferentes
// Usada para comparar string e Text
int string_equals(const char* str1, const char* str2);


#endif
// Fim de utils.h