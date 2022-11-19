;
; Ullrich von Bassewitz, 2004-05-13
;
; ___seterrno: Will set ___errno to the value in A and return zero in A. Other
;             registers aren't changed. The function is C callable, but
;             currently only called from asm code.
;

        .include        "errno.inc"

.code

.proc   ___seterrno

        sta     ___errno
        lda     #0
        sta     ___errno+1
        rts

.endproc


