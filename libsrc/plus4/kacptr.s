;
; Ullrich von Bassewitz, 22.11.2002
;
; ACPTR replacement function
;

        .export         ACPTR

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   ACPTR
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFA5                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


