;
; Oliver Schmidt, 14.08.2018
;
; int __fastcall__ clock_gettime (clockid_t clk_id, struct timespec *tp);
;

        .import         pushax, steaxspidx, incsp1, incsp3, return0

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

        ; Get date
        lda     DATELO+1
        lsr
        php                     ; Save month msb
        cmp     #70             ; Year < 70?
        bcs     :+              ; No, leave alone
        adc     #100            ; Move 19xx to 20xx
:       sta     TM + tm::tm_year
        lda     DATELO
        tax                     ; Save day
        plp                     ; Restore month msb
        ror
        lsr
        lsr
        lsr
        lsr
        beq     erange          ; [1..12] allows for validity check
        tay
        dey                     ; Move [1..12] to [0..11]
        sty     TM + tm::tm_mon
        txa                     ; Restore day
        and     #%00011111
        sta     TM + tm::tm_mday

        ; Get time
        lda     TIMELO+1
        sta     TM + tm::tm_hour
        lda     TIMELO
        sta     TM + tm::tm_min

        ; Make time_t
        lda     #<TM
        ldx     #>TM
        jsr     _mktime

        ; Store tv_sec
        ldy     #timespec::tv_sec
        jsr     steaxspidx

        ; Cleanup stack
        jsr     incsp1

        ; Return success
        jmp     return0

        ; Load errno code
erange: lda     #ERANGE

        ; Cleanup stack
        jsr     incsp3          ; Preserves A

        ; Set __errno
        jmp     __directerrno

        ; Cleanup stack
oserr:  jsr     incsp3          ; Preserves A

        ; Set __oserror
        jmp     __mappederrno

        .bss

TM:     .tag    tm
