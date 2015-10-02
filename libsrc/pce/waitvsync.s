;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync

        .forceimport    ticktock

        .include        "pce.inc"
        .include        "extzp.inc"

.proc   _waitvsync
        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts
.endproc
