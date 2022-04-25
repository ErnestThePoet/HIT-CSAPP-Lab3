#pragma once
#include "macros.h"

#define BRACKETED_LINE(S)           '['<<S<<"]\n"

#ifdef TAISHAN

#include <cstdio>
#define SET_PRINT_COLOR_RED      printf("\033[31m")
#define SET_PRINT_COLOR_GREEN    printf("\033[32m")
#define SET_PRINT_COLOR_DEFAULT  printf("\033[m")

#else

#include <Windows.h>
#define SET_PRINT_COLOR_RED \
do{ \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);\
} while(false)

#define SET_PRINT_COLOR_GREEN \
do{ \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);\
} while(false)

#define SET_PRINT_COLOR_DEFAULT \
do{ \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
    SetConsoleTextAttribute(hConsole, 7);\
} while(false)

#endif