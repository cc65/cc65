;
; int clock_settime (clockid_t clk_id, const struct timespec *tp);
;
        .include "errno.inc"
        .export _clock_settime
_clock_settime:
        lda     #ENOSYS
        jmp     __directerrno
