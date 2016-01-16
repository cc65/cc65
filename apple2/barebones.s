; Example showing how to assemble+link without any of the library crap

; ca65 doesn't have a directive to set the high bit
; as the original author didn't know anything about the Apple. :-/
; http://www.cc65.org/mailarchive/2003-05/2992.html
;
; NOTE: There is no standard directive to set the high bit on ASCII text.
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
; This macro _would_ work except ca65 is broken. :-(
.macro ASC text
    .repeat .strlen(text), I
    .byte   .strat(text, I) | $80
    .endrep
.endmacro

            COUT = $FDED

            ; 4 byte header for DOS 3.3 files
            .word $1000     ; define 2 bytes, must match org
            .word __END - * ; define 2 bytes, total size in bytes

            .org  $1000     ; .org must come after header else offsets are wrong
            LDX    #0
            LDA    MSG,X    ; load initial char
print:      ORA    #$80     ; work-around stupid macro bug *sigh*
            JSR    COUT
            INX
            LDA    MSG,X
            BNE    print
            RTS

MSG:        .asciiz "Hello, world!"

__END:

