;
; void waitvblank (void);
;

        .include        "pce.inc"

        .export         _waitvblank

.proc   _waitvblank

        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts

.endproc
