;
; Ullrich von Bassewitz, 22.11.2002
;
; RDTIM replacement function
;

        .export         RDTIM

        .include        "plus4.inc"

; Read the clock from the zero page to avoid banking in the ROM

.proc   RDTIM
        sei                             ; No interrupts
        lda     TIME+2
        ldx     TIME+1
        ldy     TIME                    ; Read the time
        cli                             ; Allow interrupts
        rts                             ; Return to caller
.endproc


