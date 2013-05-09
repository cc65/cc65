;
; Piotr Fusik, 04.11.2001
; originally by Ullrich von Bassewitz and Sidney Cadot
;
; clock_t clock (void);
; unsigned _clocks_per_sec (void);
;

        .export         _clock, __clocks_per_sec
        .importzp       sreg

        .include        "atari.inc"


.proc   _clock

        ldx     #5              ; Synchronize with Antic, so the interrupt won't change RTCLOK
        stx     WSYNC           ; while we're reading it. The synchronization is done same as
@L1:    dex                     ; in SETVBLV function in Atari OS.
        bne     @L1
        stx     sreg+1          ; Byte 3 is always zero
        lda     RTCLOK+2
        ldx     RTCLOK+1
        ldy     RTCLOK
        sty     sreg
        rts

.endproc


.proc   __clocks_per_sec

        ldx     #$00            ; Clear high byte of return value
        lda     PAL             ; use hw register, PALNTS is only supported on XL/XE ROM
        and     #$0e
        bne     @NTSC
        lda     #50
        rts
@NTSC:  lda     #60
        rts

.endproc
