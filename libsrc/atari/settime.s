;
; Oliver Schmidt, 15.08.2018
; Christian Groessler, 25.09.2018
;
; int __fastcall__ clock_settime (clockid_t clk_id, const struct timespec *tp);
;

        .import         __dos_type
        .import         incsp1, return0
        .import         sdxtry

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "atari.inc"

_clock_settime:

; cleanup stack

        jsr     incsp1          ; preserves AX

; only supported on SpartaDOS-X >= 4.40

        ldy     #SPARTADOS
        cpy     __dos_type
        bne     enosys
        ldy     SDX_VERSION
        cpy     #$44
        bcc     enosys

; create tm from tp (tv_sec) input parameter

        .assert timespec::tv_sec = 0, error
        jsr     _localtime
        sta     ptr1
        stx     ptr1+1

; set date

        ldy     #tm::tm_mday
        lda     (ptr1),y        ; get day of month
        sta     SDX_DATE        ; set day of month

        ldy     #tm::tm_mon
        lda     (ptr1),y        ; get month (0-based)
        tax
        inx                     ; move [0..11] to [1..12]
        stx     SDX_DATE+1

        ldy     #tm::tm_year
        lda     (ptr1),y        ; get year (0 = year 1900)
        cmp     #100
        bcc     :+
        sbc     #100
:       sta     SDX_DATE+2

        ldy     #tm::tm_hour
        lda     (ptr1),y        ; get hour
        sta     SDX_TIME

        ldy     #tm::tm_min
        lda     (ptr1),y        ; get minutes
        sta     SDX_TIME+1

        ldy     #tm::tm_sec
        lda     (ptr1),y        ; get seconds
        sta     SDX_TIME+2

; set new time/date (SD-X call)
; SpartaDOS-X User's Guide (4.48) states at page 145:
; "In the I_GETTD and I_SETTD procedures a set Carry-Flag means that the clock driver is
; busy at the moment. You should call the routine again."
; It goes on to mention that one should provide an upper limit on the number of calls,
; in order not to "hang". We are doing this here...

        lda     #0              ; init loop count (256)
        sta     sdxtry

try_set:lda     #SDX_CLK_DEV    ; CLK device
        sta     SDX_DEVICE
        ldy     #SDX_KD_SETTD   ; SETTD function
        jsr     SDX_KERNEL      ; do the call
        bcc     done

        dec     sdxtry
        bne     try_set

        lda     #EBUSY
        bne     drcter          ; jump always

; return success

done:   jmp     return0

; load errno code

enosys: lda     #ENOSYS
drcter: jmp     __directerrno
