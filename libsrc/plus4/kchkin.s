;
; Ullrich von Bassewitz, 22.11.2002
;
; CHKIN replacement function
;

        .export         CHKIN

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CHKIN
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFC6                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


