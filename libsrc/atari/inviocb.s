;
; set EINVAL error code and returns -1
;
        .include "errno.inc"

        .export __inviocb

__inviocb:
        lda     #<EINVAL
        jmp     __directerrno
