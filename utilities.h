#pragma once
#include "macros.h"

#define BRACKETED_LINE(S)           '['<<S<<"]\n"

#ifdef TAISHAN

#include <iostream>
#define SET_PRINT_COLOR_RED      std::cout<<"\033[31m"
#define SET_PRINT_COLOR_GREEN    std::cout<<"\033[32m"
#define SET_PRINT_COLOR_DEFAULT  std::cout<<"\033[m"

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