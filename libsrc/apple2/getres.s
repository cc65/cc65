;
; Oliver Schmidt, 15.08.2018
;
; int __fastcall__ clock_getres (clockid_t clk_id, struct timespec *res);
;

        .import         __dos_type
        .import         incsp1, return0

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

_clock_getres:
        sta     ptr1
        stx     ptr1+1

        ; Cleanup stack
        jsr     incsp1

        ; Check for ProDOS 8
        lda     __dos_type
        beq     enosys

        ; Presume day resolution
        ldx     #<day_res
        ldy     #>day_res

        ; Check for realtme clock
        lda     MACHID
        lsr     a
        bcc     :+

        ; Switch to minute resolution
        ldx     #<min_res
        ldy     #>min_res

        ; Copy timespec
:       stx     ptr2
        sty     ptr2+1
        ldy     #.sizeof(timespec)-1
:       lda     (ptr2),y
        sta     (ptr1),y
        dey
        bpl     :-

        ; Return success
        jmp     return0

        ; Load errno code
enosys: lda     #ENOSYS

        ; Set __errno
        jmp     __directerrno

        .rodata

min_res:.dword  60
        .dword  0

day_res:.dword  60 * 60 * 24
        .dword  0
