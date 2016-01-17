; Example showing how to assemble+link without any cc65's C library bloat
; to produce a barebones standalone (Apple DOS 3.3) binary file.

            .include "apple2text.inc" ; AR, AS and AT macros

            COUT = $FDED

            __MAIN = $1000
            .include "dos33.inc"  ; Apple DOS 3.3 binary file 4 byte prefix header

            LDX    #0
            LDA    MSG,X          ; load initial char
PRINTCHAR:  JSR    COUT
            INX
            LDA    MSG,X
            BNE    PRINTCHAR

; Normally, you would end with an `RTS` but if you try to BRUN our binary file
; with DOS 3.3 it doesn't properly handle the return stack.
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
;    Replace the `RTS` with `JMP $3D0` -- the reconnect DOS "warmstart" vector.
            JMP $3D0

MSG:
            ; Uncomment to see a demo of Apple text
            ;AR "ABC"            ; NORMAL : APPLE high-bit text, last char is ASCII (FLASHING)
            ;AS "Hello world"    ; NORMAL : APPLE high-bit text
            ;AT "XYZ"            ; FLASH  : ASCII text, last char is APPLE high-bit (NORMAL)
            ;AC "DEFM"           ; INVERSE: Force to be control chars0 $0..$1F (INVERSE)

            AS "Hello world, Apple!"
            .byte $8D            ; CR with high-bit set
            .byte $00
__END = *

