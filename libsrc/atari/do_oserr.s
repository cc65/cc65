;
; __do_oserror updates __oserror and errno.  Do a JMP here right after
; calling CIOV. It will return with AX set to -1 ($FFFF).  It expects the CIO
; status in Y.
;
        .include "errno.inc"

        .export __do_oserror

__do_oserror:
        tya
        jmp     __mappederrno
