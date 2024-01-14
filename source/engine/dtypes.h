#pragma once
#include <stdbool.h>

#define XSTR(x) STR(x)
#define STR(x) #x

// TODO: replace "uncontrolled" data types in whole project (mainly int)
typedef signed char sByte;         // on 3ds: s8
typedef unsigned char uByte;       // on 3ds: u8
typedef short signed int sShort;   // on 3ds: s16
typedef short unsigned int uShort; // on 3ds: u16
typedef long signed int sInt;      // on 3ds: s32
