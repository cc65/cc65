;
; Ullrich von Bassewitz, 22.11.2002
;
; CIOUT replacement function
;

        .export         CIOUT

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CIOUT
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFA8                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


