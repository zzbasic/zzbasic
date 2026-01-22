// test_cores_zz.c
#include <stdio.h>
#include "color.h"

int main() {
    printf("Cores disponíveis no ZzBasic:\n\n");
    
    printf("Cores básicas:\n");
    printf("%sBLACK%s ", BLACK, RESET);
    printf("%sRED%s ", RED, RESET);
    printf("%sGREEN%s ", GREEN, RESET);
    printf("%sYELLOW%s ", YELLOW, RESET);
    printf("%sBLUE%s ", BLUE, RESET);
    printf("%sMAGENTA%s ", MAGENTA, RESET);
    printf("%sCYAN%s ", CYAN, RESET);
    printf("%sWHITE%s\n", WHITE, RESET);
    
    printf("\nCores brilhantes:\n");
    printf("%sBRIGHT_RED%s ", BRIGHT_RED, RESET);
    printf("%sBRIGHT_GREEN%s ", BRIGHT_GREEN, RESET);
    printf("%sBRIGHT_YELLOW%s ", BRIGHT_YELLOW, RESET);
    printf("%sBRIGHT_CYAN%s ", BRIGHT_CYAN, RESET);
    
    printf("\n\nCombinações úteis:\n");
    printf("%sSUCCESS%s ", COLOR_SUCCESS, COLOR_RESET);
    printf("%sERROR%s ", COLOR_ERROR, COLOR_RESET);
    printf("%sWARNING%s ", COLOR_WARNING, COLOR_RESET);
    printf("%sINFO%s ", COLOR_INFO, COLOR_RESET);
    printf("%sDEBUG%s ", COLOR_DEBUG, COLOR_RESET);
    printf("%sHEADER%s\n", COLOR_HEADER, COLOR_RESET);
    
    printf("\nPrompt (se adicionar):\n");
    printf("%s> %s(comando aqui)\n", COLOR_PROMPT, COLOR_RESET);
    
    return 0;
}