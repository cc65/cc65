;
; Ullrich von Bassewitz, 22.11.2002
;
; CLALL replacement function
;

        .export         CLALL

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CLALL
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFE7                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


