;
; 1998-09-21, Ullrich von Bassewitz
; 2019-12-25, Greg King
;
; clock_t clock (void);
;

        .export         _clock
        .importzp       sreg

        .include        "cbm.inc"
        .macpack        cpu


.proc   _clock

; Some accelerator adaptors have CMOS ICs.

.if (.cpu & ::CPU_ISET_65SC02)
        stz     sreg + 1
.else
        lda     #$00            ; Byte 3 always is zero
        sta     sreg + 1
.endif

        jsr     RDTIM
        sty     sreg
        rts

.endproc
