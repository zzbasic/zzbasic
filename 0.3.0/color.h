#ifndef COLOR_H
#define COLOR_H

// ==================== CORES DO TEXTO ====================
#define RESET        "\033[0m"
#define BLACK        "\033[30m"
#define RED          "\033[31m"
#define GREEN        "\033[32m"
#define YELLOW       "\033[33m"
#define BLUE         "\033[34m"
#define MAGENTA      "\033[35m"
#define CYAN         "\033[36m"
#define WHITE        "\033[37m"
#define BRIGHT_BLACK "\033[90m"
#define BRIGHT_RED   "\033[91m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE  "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN  "\033[96m"
#define BRIGHT_WHITE "\033[97m"

// ==================== CORES DE FUNDO ====================
#define BG_BLACK        "\033[40m"
#define BG_RED          "\033[41m"
#define BG_GREEN        "\033[42m"
#define BG_YELLOW       "\033[43m"
#define BG_BLUE         "\033[44m"
#define BG_MAGENTA      "\033[45m"
#define BG_CYAN         "\033[46m"
#define BG_WHITE        "\033[47m"
#define BG_BRIGHT_BLACK "\033[100m"
#define BG_BRIGHT_RED   "\033[101m"
#define BG_BRIGHT_GREEN "\033[102m"
#define BG_BRIGHT_YELLOW "\033[103m"
#define BG_BRIGHT_BLUE  "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN  "\033[106m"
#define BG_BRIGHT_WHITE "\033[107m"

// ==================== ESTILOS ====================
#define BOLD          "\033[1m"
#define DIM           "\033[2m"
#define ITALIC        "\033[3m"
#define UNDERLINE     "\033[4m"
#define BLINK         "\033[5m"
#define INVERSE       "\033[7m"
#define HIDDEN        "\033[8m"
#define STRIKETHROUGH "\033[9m"

// ==================== COMBINAÇÕES ÚTEIS ====================
#define COLOR_RESET 	"\033[0m"
#define COLOR_SUCCESS   "\033[1;32m"    // Verde brilhante em negrito
#define COLOR_ERROR 	"\033[1;31m"    // Vermelho brilhante em negrito
#define COLOR_WARNING 	"\033[1;33m"	// Amarelo brilhante em negrito
#define COLOR_INFO      "\033[1;36m"    // Ciano brilhante 
#define COLOR_DEBUG     "\033[1;35m"    // Magenta brilhante 
#define COLOR_HEADER    "\033[96m"      // Ciano brilhante 
#define COLOR_PROMPT    "\033[1;32m"    // Verde brilhante em negrito (para o ">")
#define COLOR_EXPR      "\033[1;37m"    // Branco brilhante (para expressões)
#define COLOR_AST       "\033[0;36m"    // Ciano normal (para AST)
#endif