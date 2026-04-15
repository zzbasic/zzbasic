// color_mapping.c - IMPLEMENTAÇÃO DO MAPEAMENTO

#include <string.h>
#include "color_mapping.h"
#include "color_constants.h"

// Mapeamento de ColorCode para sequência ANSI
static const char* color_code_to_ansi_map[] =
{
    [CLR_NOCOLOR]   = "\033[0m",
    
    // Cores normais
    [CLR_BLACK]     = "\033[30m",
    [CLR_RED]       = "\033[31m",
    [CLR_GREEN]     = "\033[32m",
    [CLR_YELLOW]    = "\033[33m",
    [CLR_BLUE]      = "\033[34m",
    [CLR_MAGENTA]   = "\033[35m",
    [CLR_CYAN]      = "\033[36m",
    [CLR_WHITE]     = "\033[37m",
    
    // Cores bright
    [CLR_BRED]      = "\033[91m",
    [CLR_BGREEN]    = "\033[92m",
    [CLR_BYELLOW]   = "\033[93m",
    [CLR_BBLUE]     = "\033[94m",
    [CLR_BMAGENTA]  = "\033[95m",
    [CLR_BCYAN]     = "\033[96m",
    [CLR_BWHITE]    = "\033[97m",
    
    // Backgrounds
    [CLR_BGBLACK]   = "\033[40m",
    [CLR_BGRED]     = "\033[41m",
    [CLR_BGGREEN]   = "\033[42m",
    [CLR_BGYELLOW]  = "\033[43m",
    [CLR_BGBLUE]    = "\033[44m",
    [CLR_BGMAGENTA] = "\033[45m",
    [CLR_BGCYAN]    = "\033[46m",
    [CLR_BGWHITE]   = "\033[47m",
};

// ============================================
// CONVERSÃO: TokenType -> ColorCode
// ============================================
ColorCode token_to_color_code(TokenType token) {
    // TOKEN_NOCOLOR = 25, então:
    // color_code = token - 25
    
    if (token >= TOKEN_NOCOLOR && token <= TOKEN_BGWHITE)
    {
        return (ColorCode)(token - TOKEN_NOCOLOR);  // 25-25=0, 26-25=1, etc
    }
    return CLR_NOCOLOR;  // Default
}


// ============================================
// CONVERSÃO: ColorCode -> ANSI string
// ============================================
const char* color_code_to_ansi(ColorCode code)
{
    if (code >= 0 && code < CLR_COUNT) {
        return color_code_to_ansi_map[code];
    }
    return "\033[0m";  // Fallback
}


// ============================================
// CONVERSÃO: TokenType -> ANSI string (direta)
// ============================================

const char* token_to_ansi(TokenType token) {
    ColorCode code = token_to_color_code(token);
    return color_code_to_ansi(code);
}
// Fim de color_mapping.c
