; Example showing how to assemble+link without any cc65's C library bloat
; to produce a barbones standalone (Apple DOS 3.3) binary file.

            .include "apple2text.inc" ; AR, AS and AT macros

            COUT = $FDED

; The operator '*' is buggy. This will generate a bogus link error:
; ld65: Error: Range error in module `link_bug.s', line 3
;            __MAIN = $1000       ; Apple DOS 3.3 binary file 4 byte prefix header
;            .word __MAIN         ; 2 byte BLAOD address
;            .word __END - __MAIN ; 2 byte BLOAD size
;
; Solution 1 is to pad a dummy byte onto the end:
;     __END:
;     .asciiz ""
;
; Solution 2 is to not use the buggy '*' operator, replace with a real variable
;     .wordwd __END - __MAIN

            __MAIN = $1000
            .include "dos33.inc"  ; Apple DOS 3.3 binary file 4 byte prefix header

            LDX    #0
            LDA    MSG,X        ; load initial char
PRINTCHAR:  JSR    COUT
            INX
            LDA    MSG,X
            BNE    PRINTCHAR

; Normally you would end with an RTS
; but if you try to BRUN our binary file with DOS 3.3
; it doesn't properly handle the return stack causing the program to run twice.
;
; Solution 1:
;    BLOAD <file>
;    CALL-151
;    AA72.AA73
;    Swap the byte displayed
;    To run type:
;        ####G
;    i.e. 1000G
;
; Solution 2:
;    Replace the RTS with 'JMP $3D0' which is the reconnect DOS vector.
            JMP $3D0

MSG:
            ; = Apple Text Demo =
            ;AR "ABC"            ; NORMAL : APPLE high-bit text, last char is ASCII (FLASHING)
            ;AS "Hello world"    ; NORMAL : APPLE high-bit text
            ;AT "XYZ"            ; FLASH  : ASCII text, last char is APPLE high-bit (NORMAL)
            ;AC "DEFM"           ; INVERSE: Force to be control chars0 $0..$1F (INVERSE)

            AS "Hello world, Apple!"
            .byte $8D           ; CR with high-bit set
            .byte $00
__END = *

