;
; 2019-12-27, Greg King
;
; int __fastcall__ clock_settime (clockid_t clk_id, const struct timespec *tp);
;

        .include        "time.inc"
        .include        "cx16.inc"

        .importzp       ptr1
        .import         pushax, pusheax, ldax0sp, ldeaxidx
        .import         tosdiveax, incsp3, return0
        .import         load_jiffy
        .import         CLOCK_SET_DATE_TIME


.macro  COPY    reg, offset
        ldy     #offset
        lda     (ptr1),y
        sta     gREG::reg
.endmac

;----------------------------------------------------------------------------

.proc   _clock_settime

        jsr     pushax

        .assert timespec::tv_sec = 0, error
        jsr     _localtime
        sta     ptr1
        stx     ptr1+1

        COPY    r0L, tm::tm_year
        COPY    r0H, tm::tm_mon
        inc     gREG::r0H
        COPY    r1L, tm::tm_mday
        COPY    r1H, tm::tm_hour
        COPY    r2L, tm::tm_min
        COPY    r2H, tm::tm_sec

        jsr     ldax0sp                 ; Get tp
        ldy     #timespec::tv_nsec+3
        jsr     ldeaxidx                ; Get nanoseconds
        jsr     pusheax
        jsr     load_jiffy
        jsr     tosdiveax
        sta     gREG::r3L               ; Put number of jiffies

        jsr     CLOCK_SET_DATE_TIME

        jsr     incsp3
        jmp     return0

.endproc
