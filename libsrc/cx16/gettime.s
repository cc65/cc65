;
; 2019-12-27, Greg King
;
; int __fastcall__ clock_gettime (clockid_t clk_id, struct timespec *tp);
;

        .include        "time.inc"
        .include        "cx16.inc"

        .import         pushax, pusheax, tosmul0ax, steaxspidx, incsp1, return0
        .import         TM, load_jiffy
        .import         CLOCK_GET_DATE_TIME


;----------------------------------------------------------------------------

.proc   _clock_gettime

        jsr     pushax
        jsr     pushax

        jsr     CLOCK_GET_DATE_TIME

        lda     gREG::r0L
        sta     TM + tm::tm_year
        lda     gREG::r0H
        dec     a
        sta     TM + tm::tm_mon
        lda     gREG::r1L
        sta     TM + tm::tm_mday

        lda     gREG::r1H
        sta     TM + tm::tm_hour
        lda     gREG::r2L
        sta     TM + tm::tm_min
        lda     gREG::r2H
        sta     TM + tm::tm_sec

        lda     #<TM
        ldx     #>TM
        jsr     _mktime
        ldy     #timespec::tv_sec
        jsr     steaxspidx              ; Pops address pushed by 2. pushax

        jsr     load_jiffy
        jsr     pusheax
        lda     gREG::r3L
        ldx     #>$0000
        jsr     tosmul0ax
        ldy     #timespec::tv_nsec
        jsr     steaxspidx              ; Pops address pushed by 1. pushax

        jsr     incsp1
        jmp     return0

.endproc
