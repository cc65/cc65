;
; Oliver Schmidt, 14.08.2018
; Christian Groessler, 25.09.2018
;
; int __fastcall__ clock_gettime (clockid_t clk_id, struct timespec *tp);
;

        .import         pushax, steaxspidx, incsp1, incsp3, return0
        .import         __dos_type
        .import         sdxtry

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "atari.inc"

_clock_gettime:
        jsr     pushax

; clear tp

        sta     ptr1
        stx     ptr1+1
        lda     #$00
        ldy     #.sizeof(timespec)-1
:       sta     (ptr1),y
        dey
        bpl     :-

; only supported on SpartaDOS-X >= 4.40

        lda     #SPARTADOS
        cmp     __dos_type
        bne     notsupp
        lda     SDX_VERSION
        cmp     #$44
        bcc     notsupp

; get date/time from system (SD-X call)
; see settime.s for reasons of using sdxtry

        lda     #0              ; init loop count (256)
        sta     sdxtry

try_get:lda     #SDX_CLK_DEV    ; CLK device
        sta     SDX_DEVICE
        ldy     #SDX_KD_GETTD   ; GETTD function
        jsr     SDX_KERNEL      ; do the call
        bcc     done

        dec     sdxtry
        bne     try_get

        lda     #EBUSY
        bne     errexit

; fill timespec

; date
done:   lda     SDX_DATE        ; mday
        sta     TM + tm::tm_mday
        ldx     SDX_DATE+1      ; month
        dex
        stx     TM + tm::tm_mon
        lda     SDX_DATE+2      ; year
        cmp     #79             ; 1979: the Atari 800 came out
        bcs     :+
        adc     #100            ; adjust century
:       sta     TM + tm::tm_year

; time
        lda     SDX_TIME
        sta     TM + tm::tm_hour
        lda     SDX_TIME+1
        sta     TM + tm::tm_min
        lda     SDX_TIME+2
        sta     TM + tm::tm_sec

; make time_t

        lda     #<TM
        ldx     #>TM
        jsr     _mktime

; store tv_sec into output tp struct

        ldy     #timespec::tv_sec
        jsr     steaxspidx

; cleanup stack

        jsr     incsp1

; return success

        jmp     return0

; load errno code

notsupp:lda     #ENOSYS

; cleanup stack

errexit:jsr     incsp3          ; Preserves A

; set __errno

        jmp     __directerrno

; -------

        .bss

TM:     .tag    tm
