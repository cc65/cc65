;
; Oliver Schmidt, 15.8.2018
;
; int __fastcall__ clock_getres (clockid_t clk_id, struct timespec *res);
;

        .include        "time.inc"

        .importzp       ptr1
        .import         incsp1, return0


;----------------------------------------------------------------------------
.code

.proc   _clock_getres

        sta     ptr1
        stx     ptr1+1

        ldy     #.sizeof(timespec)-1
@L1:    lda     time,y
        sta     (ptr1),y
        dey
        bpl     @L1

        jsr     incsp1
        jmp     return0

.endproc

;----------------------------------------------------------------------------
; timespec struct with tv_nsec set to 1/10 second
.rodata

time:   .dword  0
        .dword  100 * 1000 * 1000
