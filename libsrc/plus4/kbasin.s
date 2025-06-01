;
; Ullrich von Bassewitz, 22.11.2002
;
; BASIN/CHRIN replacement function
;

        .export         BASIN
        .export         CHRIN

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   BASIN
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFCF                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc

CHRIN   :=      BASIN
