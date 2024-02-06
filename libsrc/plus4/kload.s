;
; Ullrich von Bassewitz, 22.11.2002
;
; LOAD replacement function
;

        .export         LOAD

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   LOAD
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFD5                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


