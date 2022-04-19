
        .include        "atari7800.inc"

        .export _mono_clrscr

        .import _mono_screen
        .import pushax, __bzero
        .include "extzp.inc"

        .code

        .proc _mono_clrscr

        lda     #<_mono_screen
        ldx     #>_mono_screen
        jsr     pushax
        ldx     #>(mono_charsperline * screenrows)
        lda     #<(mono_charsperline * screenrows)
        jmp     __bzero

        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import mono_initconio
conio_init      = mono_initconio
