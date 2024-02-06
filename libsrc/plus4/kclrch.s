;
; Ullrich von Bassewitz, 22.11.2002
;
; CLRCH replacement function
;

        .export         CLRCH

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CLRCH
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFCC                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


