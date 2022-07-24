;
; Oliver Schmidt, 15.08.2018
;
; int __fastcall__ clock_settime (clockid_t clk_id, const struct timespec *tp);
;

        .import         __dos_type
        .import         incsp1, return0

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

_clock_settime:

        ; Cleanup stack
        jsr     incsp1          ; Preserves A

        ; Check for ProDOS 8
        ldy     __dos_type
        beq     enosys

        ; Check for realtme clock
        tay                     ; Save A
        lda     MACHID
        lsr     a
        bcs     erange
        tya                     ; Restore A

        ; Get tm
        .assert timespec::tv_sec = 0, error
        jsr     _localtime
        sta     ptr1
        stx     ptr1+1

        ; Set date
        ldy     #tm::tm_mon
        lda     (ptr1),y
        clc
        adc     #$01            ; Move [0..11] to [1..12]
        asl
        asl
        asl
        asl
        asl
        php                     ; Save month msb
        ldy     #tm::tm_mday
        ora     (ptr1),y
        sta     DATELO
        ldy     #tm::tm_year
        lda     (ptr1),y
        cmp     #100            ; Year since 1900 < 100?
        bcc     :+              ; Yes, leave alone
        sbc     #100            ; Move 20xx to 19xx
:       plp                     ; Restore month msb
        rol
        sta     DATELO+1

        ; Return success
        jmp     return0

        ; Load errno code
enosys: lda     #ENOSYS
        bne     errno           ; Always

        ; Load errno code
erange: lda     #ERANGE

        ; Set __errno
errno:  jmp     __directerrno
