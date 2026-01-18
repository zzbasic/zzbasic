#ifndef UTILS_H
#define UTILS_H

#define BUFFER_SIZE       256    // Para mensagens de erro LONGAS
#define TOKEN_TEXT_SIZE   128     // Para texto de token (números, operadores)
#define MAX_NUMBER_LEN    63     // Específico do lexer

void setup_utf8();

void wait();

#endif