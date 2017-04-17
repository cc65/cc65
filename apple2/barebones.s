; Example showing how to assemble+link without any of the cc65 library crap
; to produce a standalone Apple DOS3.3 binary barbones file
;
; ca65 doesn't have a directive to set the high bit on text
; as the original author didn't know anything about the Apple. :-/
; http://www.cc65.org/mailarchive/2003-05/2992.html
;
; NOTE: There is no "standard" directive to set the high bit on ASCII text.
;
; * The assembler used in the redbook uses `msb on` and `msb off`
; * S-C Macro Assembler used
;     .AS for normal ASCII, and
;     .AT for normal ASCII but the last char has the high bit ON
; * Merlin uses yet another variation:
;     ASC 'Hello'   ; high bit is off
;     ASC "Hello"   ; high bit is ON
;   See the new Merlin-32 project
;   http://brutaldeluxe.fr/products/crossdevtools/merlin/
; 
; "The 'nice' thing about standards, is that there are so many to pick from!"
;
; NOTE: If you use the buggy '*' operator (see note below) this macro won't
; process the last 4 chars! Once we used a working "END-MAIN" we no longer
; need to manually set the high bit when printing characters via `ORA #80`.
.macro ASC text
    .repeat .strlen(text), I
    .byte   .strat(text, I) | $80
    .endrep
.endmacro

            COUT = $FDED

; The operator '*' is buggy. This will generate a bogusBogus link error:
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
            .include "dos33.h"  ; Apple DOS 3.3 binary file 4 byte prefix header

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
            ASC "Hello world, Apple"
            .byte "!"           ; normal ASCII displayed as inverse
            .byte $8D           ; CR with high-bit set
            .byte $00
__END:

