;
; 2002-11-22, Ullrich von Bassewitz
; 2016-08-07, Greg King
;
; SCNKEY replacement function
;

        .export         SCNKEY

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   SCNKEY
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FF9F                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc
