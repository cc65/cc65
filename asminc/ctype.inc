; ctype.inc
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; Definitions for the character type tables
;
; Ullrich von Bassewitz, 08.09.2001
;

; Define bitmapped constants for the table entries

CT_NONE         = %00000000     ; Nothing special
CT_LOWER        = %00000001     ; 0 - Lower case char
CT_UPPER        = %00000010     ; 1 - Upper case char
CT_DIGIT        = %00000100     ; 2 - Numeric digit
CT_XDIGIT       = %00001000     ; 3 - Hex digit (both, lower and upper)
CT_CTRL         = %00010000     ; 4 - Control character
CT_SPACE        = %00100000     ; 5 - The space character itself
CT_OTHER_WS     = %01000000     ; 6 - Other whitespace ('\f', '\n', '\r', '\t' and '\v')
CT_SPACE_TAB    = %10000000     ; 7 - Space or tab character

; Combined stuff
CT_ALNUM        = (CT_LOWER | CT_UPPER | CT_DIGIT)
CT_ALPHA        = (CT_LOWER | CT_UPPER)
CT_CTRL_SPACE   = (CT_CTRL | CT_SPACE)
CT_NOT_PUNCT    = (CT_SPACE | CT_CTRL | CT_DIGIT | CT_UPPER | CT_LOWER)
