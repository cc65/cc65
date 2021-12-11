// lib_common_ctype.c
//
// This file is part of
// cc65 - a freeware C compiler for 6502 based systems
//
// https://cc65.github.io
//
// See "LICENSE" file for legal information.
//
// Unit test for character classification functions ("is..")
//

#include <ctype.h>
#include <stdbool.h>
#include "unittest.h"

#define NUMTESTS 257

typedef struct 
{
    bool isalnum;
    bool isalpha;
    bool isascii;
    bool iscntrl;
    bool isdigit;
    bool isgraph;
    bool islower;
    bool isprint;
    bool ispunct;
    bool isspace;
    bool isupper;
    bool isxdigit;
    bool isblank; 

} CTypeClassifications;


CTypeClassifications testSet[NUMTESTS] =
{
   //alnum, alpha, ascii, cntrl, digit, graph, lower, print, punct, space, upper, xdigit,blank

    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 00
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 01
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 02
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 03
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 04
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 05
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 06
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 07
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 08
    {false, false, true,  true,  false, false, false, false, false, true,  false, false, true },        // 09
    {false, false, true,  true,  false, false, false, false, false, true,  false, false, false},        // 0A
    {false, false, true,  true,  false, false, false, false, false, true,  false, false, false},        // 0B
    {false, false, true,  true,  false, false, false, false, false, true,  false, false, false},        // 0C
    {false, false, true,  true,  false, false, false, false, false, true,  false, false, false},        // 0D
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 0E
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 0F

    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 10
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 11
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 12
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 13
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 14
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 15
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 16
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 17
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 18
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 19
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1A
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1B
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1C
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1D
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1E
    {false, false, true,  true,  false, false, false, false, false, false, false, false, false},        // 1F

    {false, false, true,  false, false, false, false, true,  false, true,  false, false, true },        // 20
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 21
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 22
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 23
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 24
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 25
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 26
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 27
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 28
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 29
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2A
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2B
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2C
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2D
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2E
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 2F
 
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 30
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 31
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 32
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 33
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 34
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 35
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 36
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 37
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 38
    {true,  false, true,  false, true,  true,  false, true,  false, false, false, true,  false},        // 39
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3A
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3B
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3C
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3D
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3E
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 3F

    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 40
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 41
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 42
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 43
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 44
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 45
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  true,  false},        // 46
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 47
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 48
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 49
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4A
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4B
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4C
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4D
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4E
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 4F
 
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 50
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 51
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 52
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 53
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 54
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 55
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 56
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 57
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 58
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 59
    {true,  true,  true,  false, false, true,  false, true,  false, false, true,  false, false},        // 5A
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 5B
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 5C
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 5D
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 5E
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 5F

    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 60
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 61
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 62
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 63
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 64
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 65
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, true,  false},        // 66
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 67
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 68
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 69
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6A
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6B
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6C
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6D
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6E
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 6F

    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 70
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 71
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 72
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 73
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 74
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 75
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 76
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 77
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 78
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 79
    {true,  true,  true,  false, false, true,  true,  true,  false, false, false, false, false},        // 7A
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 7B
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 7C
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 7D
    {false, false, true,  false, false, true,  false, true,  true,  false, false, false, false},        // 7E
    {false, false, true,  false, false, true,  false, true,  true,  true,  false, false, false},        // 7F

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 80
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 81
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 82
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 83
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 84
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 85
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 86
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 87
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 88
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 89
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8A
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8B
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8C
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8D
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8E
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 8F

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 90
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 91
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 92
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 93
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 94
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 95
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 96
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 97
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 98
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 99
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9A
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9B
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9C
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9D
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9E
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // 9F

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // A9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AD
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // AF

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // B9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BD
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // BF

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // C9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CD
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // CF

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // D9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DD
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // DF

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // E9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // EA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // EB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // EC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // ED
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // EE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // EF

    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F0
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F1
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F2
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F3
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F4
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F5
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F6
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F7
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F8
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // F9
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FA
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FB
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FC
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FD
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FE
    {false, false, false, false, false, true,  false, true,  true,  false, false, false, false},        // FF

    // out of range test
    {false, false, false, false, false, false, false, false, false, false, false, false, false}         // 100
};


TEST
{
    int i = 0;

    while (i<NUMTESTS)
    {
        // isalnum()
        ASSERT_AreEqual(testSet[i].isalnum, (isalnum(i) ? true : false), "%d", "Invalid 'isalnum(%d)' classification!" COMMA i);

        // isalpha()
        ASSERT_AreEqual(testSet[i].isalpha, (isalpha(i) ? true : false), "%d", "Invalid 'isalpha(%d)' classification!" COMMA i);

        // isascii()
        ASSERT_AreEqual(testSet[i].isascii, (isascii(i) ? true : false), "%d", "Invalid 'isascii(%d)' classification!" COMMA i);

        // iscntrl()
        ASSERT_AreEqual(testSet[i].iscntrl, (iscntrl(i) ? true : false), "%d", "Invalid 'iscntrl(%d)' classification!" COMMA i);

        // isdigit()
        ASSERT_AreEqual(testSet[i].isdigit, (isdigit(i) ? true : false), "%d", "Invalid 'isdigit(%d)' classification!" COMMA i);

        // isgraph()
        ASSERT_AreEqual(testSet[i].isgraph, (isgraph(i) ? true : false), "%d", "Invalid 'isgraph(%d)' classification!" COMMA i);

        // islower()
        ASSERT_AreEqual(testSet[i].islower, (islower(i) ? true : false), "%d", "Invalid 'islower(%d)' classification!" COMMA i);

        // isprint()
        ASSERT_AreEqual(testSet[i].isprint, (isprint(i) ? true : false), "%d", "Invalid 'isprint(%d)' classification!" COMMA i);

        // ispunct()
        ASSERT_AreEqual(testSet[i].ispunct, (ispunct(i) ? true : false), "%d", "Invalid 'ispunct(%d)' classification!" COMMA i);

        // isspace()
        ASSERT_AreEqual(testSet[i].isspace, (isspace(i) ? true : false), "%d", "Invalid 'isspace(%d)' classification!" COMMA i);

        // isupper()
        ASSERT_AreEqual(testSet[i].isupper, (isupper(i) ? true : false), "%d", "Invalid 'isupper(%d)' classification!" COMMA i);

        // isxdigit()
        ASSERT_AreEqual(testSet[i].isxdigit, (isxdigit(i) ? true : false), "%d", "Invalid 'isxdigit(%d)' classification!" COMMA i);

#if __CC65_STD__ >= __CC65_STD_C99__
        // isblank()
        ASSERT_AreEqual(testSet[i].isblank, (isblank(i) ? true : false), "%d", "Invalid 'isblank(%d)' classification!" COMMA i);
#endif
        ++i;
    }
}
ENDTEST
