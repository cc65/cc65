
        .export _getsp
        .importzp       c_sp

.proc   _getsp

        ldx     c_sp+1
        lda     c_sp
        rts

.endproc

