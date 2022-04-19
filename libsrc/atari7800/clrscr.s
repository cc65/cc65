
        .include        "atari7800.inc"

        .export _clrscr

        .import _screen
        .import pushax, __bzero
        .include "extzp.inc"

        .code

        .proc _clrscr

        lda     #<_screen
        ldx     #>_screen
        jsr     pushax
        ldx     #>(charsperline * screenrows)
        lda     #<(charsperline * screenrows)
        jmp     __bzero

        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
