;
; Ullrich von Bassewitz, 22.11.2002
;
; UNLSN replacement function
;

        .export         UNLSN

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   UNLSN
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFAE                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


