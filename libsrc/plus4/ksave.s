;
; Ullrich von Bassewitz, 22.11.2002
;
; SAVE replacement function
;

        .export         SAVE

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   SAVE
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFD8                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


