;
; Written by Christian Groessler <chris@groessler.org>
;
; void waitvsync (void);
;

        .include        "atari.inc"
        .export         _waitvsync

.proc   _waitvsync
        lda     RTCLOK+2
@lp:    cmp     RTCLOK+2
        beq     @lp
        rts
.endproc
