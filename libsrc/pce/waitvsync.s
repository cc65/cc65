;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync

        .forceimport    ticktock        ; make sure that tickcount changes

        .include        "extzp.inc"

.proc   _waitvsync
        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts
.endproc
