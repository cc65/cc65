;
; Ullrich von Bassewitz, 22.11.2002
;
; LISTEN replacement function
;

        .export         LISTEN

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   LISTEN
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFB1                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


