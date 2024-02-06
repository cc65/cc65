;
; Ullrich von Bassewitz, 22.11.2002
;
; TALK replacement function
;

        .export         TALK

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   TALK
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFB4                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


