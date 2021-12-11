;
; Written by Christian Groessler <chris@groessler.org>
;
; void waitvsync (void);
;

        .include        "atari5200.inc"
        .export         _waitvsync

.proc   _waitvsync
        lda     RTCLOK+1
@lp:    cmp     RTCLOK+1
        beq     @lp
        rts
.endproc
