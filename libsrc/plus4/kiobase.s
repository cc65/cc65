;
; Ullrich von Bassewitz, 22.11.2002
;
; IOBASE replacement function
;

        .export         IOBASE

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   IOBASE
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFF3                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


