
        .export _getsp
        .importzp       sp

.proc   _getsp

        ldx     sp+1
        lda     sp
        rts

.endproc

