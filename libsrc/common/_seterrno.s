;
; Ullrich von Bassewitz, 2004-05-13
;
; __seterrno: Will set __errno to the value in A and return zero in A. Other
;             registers aren't changed. The function is C callable, but 
;             currently only called from asm code.
;

        .include        "errno.inc"

.code

.proc   __seterrno

        sta     __errno
        lda     #0
        sta     __errno+1
        rts

.endproc


