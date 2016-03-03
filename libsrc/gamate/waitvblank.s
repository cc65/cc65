;
; void waitvblank (void);
;

        .include        "gamate.inc"
        .include        "extzp.inc"

        .forceimport    ticktock
        .export         _waitvblank

; FIXME: is this actually correct?

.proc   _waitvblank

        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts

.endproc
