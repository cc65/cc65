;
; void waitvsync (void);
;

        .include        "pce.inc"
        .include        "extzp.inc"

        .forceimport    ticktock
        .export         _waitvsync

.proc   _waitvsync

        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts

.endproc
