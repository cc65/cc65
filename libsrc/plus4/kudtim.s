;
; 2002-11-22, Ullrich von Bassewitz
; 2016-08-07, Greg King
;
; UDTIM replacement function
;

        .export         UDTIM

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   UDTIM
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFEA                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc
