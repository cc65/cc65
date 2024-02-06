;
; Ullrich von Bassewitz, 22.11.2002
;
; OPEN replacement function
;

        .export         OPEN

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   OPEN
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFC0                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


