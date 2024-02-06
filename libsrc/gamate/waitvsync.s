;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .include        "gamate.inc"
        .include        "extzp.inc"

        .forceimport    ticktock
        .export         _waitvsync

; FIXME: is this actually correct?

.proc   _waitvsync

        lda     tickcount
@lp:    cmp     tickcount
        beq     @lp
        rts

.endproc
