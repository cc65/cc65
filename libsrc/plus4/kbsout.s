;
; Ullrich von Bassewitz, 22.11.2002
;
; BSOUT replacement function
;

        .export         BSOUT

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   BSOUT
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFD2                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


