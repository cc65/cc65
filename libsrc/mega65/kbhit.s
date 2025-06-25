

       ; FIXME: is $d610 mega65 specific?
       ; FIXME: this should rather use the kernal (with keyboard buffer etc)

        .export _cgetc
_cgetc:

:       lda $d610
        beq :-
        ldx #0
        stx $d610
        rts

        .export _kbhit
_kbhit:
        lda $d610
        beq :+

        lda #1
:
        ldx #>$0000
        rts

