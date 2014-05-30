;
; from Atari computer version by Christian Groessler, 2014
;
; clock_t clock (void);
; unsigned _clocks_per_sec (void);
;

        .export         _clock
        .importzp       sreg

        .include        "atari5200.inc"


.proc   _clock

        ldx     #5              ; Synchronize with Antic, so the interrupt won't change RTCLOK
        stx     WSYNC           ; while we're reading it. The synchronization is done same as
@L1:    dex                     ; in SETVBLV function in Atari OS.
        bne     @L1
        stx     sreg+1          ; Byte 3 is always zero
        stx     sreg            ; Byte 2 is always zero, too
        lda     RTCLOK+1
        ldx     RTCLOK
        rts

.endproc
