
        .export _getsp
        .importzp       spc

.proc   _getsp

        ldx     spc+1
        lda     spc
        rts

.endproc

