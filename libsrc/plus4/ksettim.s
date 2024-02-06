;
; Ullrich von Bassewitz, 22.11.2002
;
; SETTIM replacement function
;

        .export         SETTIM

        .include        "plus4.inc"

; Set the clock by writing directly to zero page to avoid banking in the ROM

.proc   SETTIM
        sei                             ; No interrupts
        sta     TIME+2
        stx     TIME+1
        sty     TIME                    ; Set the time
        cli                             ; Allow interrupts
        rts                             ; Return to caller
.endproc


