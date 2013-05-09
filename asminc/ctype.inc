;
; Definitions for the character type tables
;
; Ullrich von Bassewitz, 08.09.2001
;

; Make the __ctype table an exported/imported symbol

        .global    __ctype

; Define bitmapped constants for the table entries

CT_NONE         = $00           ; Nothing special
CT_LOWER        = $01           ; 0 - Lower case char
CT_UPPER        = $02           ; 1 - Upper case char
CT_DIGIT        = $04           ; 2 - Numeric digit
CT_XDIGIT       = $08           ; 3 - Hex digit (both, lower and upper)
CT_CTRL         = $10           ; 4 - Control character
CT_SPACE        = $20           ; 5 - The space character itself
CT_OTHER_WS     = $40           ; 6 - Other whitespace ('\f', '\n', '\r', '\t' and '\v')
CT_SPACE_TAB    = $80           ; 7 - Space or tab character

; Combined stuff
CT_ALNUM        = (CT_LOWER | CT_UPPER | CT_DIGIT)
CT_ALPHA        = (CT_LOWER | CT_UPPER)
CT_CTRL_SPACE   = (CT_CTRL | CT_SPACE)
CT_NOT_PUNCT    = (CT_SPACE | CT_CTRL | CT_DIGIT | CT_UPPER | CT_LOWER)


