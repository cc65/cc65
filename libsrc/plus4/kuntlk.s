;
; Ullrich von Bassewitz, 22.11.2002
;
; UNTLK replacement function
;

        .export         UNTLK

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   UNTLK
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFAB                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


