;
; Oliver Schmidt, 14.08.2018
;
; int __fastcall__ clock_gettime (clockid_t clk_id, struct timespec *tp);
;

        .import         pushax, incsp1, incsp3, steaxspidx, return0
        .import         _mktime_dt

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

_clock_gettime:
        jsr     pushax

        ; Clear tv_nsec (+ tv_sec)
        sta     ptr1
        stx     ptr1+1
        lda     #$00
        ldy     #.sizeof(timespec)-1
:       sta     (ptr1),y
        dey
        bpl     :-

        ; Update date + time
        lda     #GET_TIME_CALL
        ldx     #GET_TIME_COUNT
        jsr     callmli
        bcs     oserr

        ; Convert DATELO/TIMELO to time_t
        lda     #<DATELO
        ldx     #>DATELO
        jsr     _mktime_dt

        ; Store
        ldy     #timespec::tv_sec
        jsr     steaxspidx

        ; Cleanup stack
        jsr     incsp1

        ; Return success
        jmp     return0

        ; Cleanup stack
oserr:  jsr     incsp3          ; Preserves A

        ; Set ___oserror
        jmp     ___mappederrno
