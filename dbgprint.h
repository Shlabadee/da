/*
    DO NOT USE clang-format ON THIS FILE
*/

#ifndef DBGPRINT
    #ifdef DEBUG
        #include <stdio.h>
        #define DBGPRINT printf
    #else
        #define DBGPRINT(...) do {} while(0)
    #endif
#endif