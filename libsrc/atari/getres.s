;
; Oliver Schmidt, 15.8.2018
; Christian Groessler, 27.9.2018
;
; int __fastcall__ clock_getres (clockid_t clk_id, struct timespec *res);
;

        .include        "atari.inc"
        .include        "time.inc"
        .include        "errno.inc"

        .importzp       ptr1
        .import         incsp1, return0, __dos_type

;----------------------------------------------------------------------------
.code

_clock_getres:
        sta     ptr1
        stx     ptr1+1

        ; Cleanup stack
        jsr     incsp1

        ; Check for SpartaDOS-X 4.40 or newer
        ldy     #SPARTADOS
        cpy     __dos_type
        bne     enosys
        ldy     SDX_VERSION
        cpy     #$44
        bcc     enosys

        ldy     #.sizeof(timespec)-1
@L1:    lda     time,y
        sta     (ptr1),y
        dey
        bpl     @L1

        jmp     return0

enosys: lda     #ENOSYS

        ; Set __errno
        jmp     __directerrno

;----------------------------------------------------------------------------
; timespec struct with tv_sec set to 1 second
.rodata

time:   .dword  1
        .dword  0
