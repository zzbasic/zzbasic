// color_constants.h - CONSTANTES COMPARTILHADAS

#ifndef COLOR_CONSTANTS_H
#define COLOR_CONSTANTS_H

// Códigos de cores (independentes de TokenType)
typedef enum {
    CLR_NOCOLOR,
    
    // Cores normais (8)
    CLR_BLACK,
    CLR_RED,
    CLR_GREEN,
    CLR_YELLOW,
    CLR_BLUE,
    CLR_MAGENTA,
    CLR_CYAN,
    CLR_WHITE,
    
    // Cores bright (8)
    CLR_BRED,
    CLR_BGREEN,
    CLR_BYELLOW,
    CLR_BBLUE,
    CLR_BMAGENTA,
    CLR_BCYAN,
    CLR_BWHITE,
    
    // Background colors (8 - futuro)
    CLR_BGBLACK,
    CLR_BGRED,
    CLR_BGGREEN,
    CLR_BGYELLOW,
    CLR_BGBLUE,
    CLR_BGMAGENTA,
    CLR_BGCYAN,
    CLR_BGWHITE,
    
    CLR_COUNT  // Total de cores (para validação)
} ColorCode;

#endif // COLOR_CONSTANTS_H