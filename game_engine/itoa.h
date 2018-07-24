/**
* C++ version 0.4 char* style "itoa":
* Written by Lukas Chmela
* Released under GPLv3.
*/


#pragma once

#ifndef _ITOA
#define _ITOA


char* __itoa(int value, char* result, int base);
char* __itoa(unsigned int value, char* result, int base);
char* __itoa( __int64 value, char* result, int base);


#endif //_ITOA