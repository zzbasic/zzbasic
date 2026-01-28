// color_mapping.h - MAPEAMENTO ENTRE TOKEN_TYPE E COLOR_CODE

#ifndef COLOR_MAPPING_H
#define COLOR_MAPPING_H

#include "lexer.h"       		// Para TokenType
#include "color_constants.h" 	// Para ColorCode  

// Função para converter TokenType para ColorCode
ColorCode token_to_color_code(TokenType token);

// Função para obter sequência ANSI a partir de ColorCode
const char* color_code_to_ansi(ColorCode  code);

// Função para obter sequência ANSI diretamente de TokenType
const char* token_to_ansi(TokenType token);

#endif // COLOR_MAPPING_H