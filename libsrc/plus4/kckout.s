;
; Ullrich von Bassewitz, 22.11.2002
;
; CKOUT replacement function
;

        .export         CKOUT

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CKOUT
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     $FFC9                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


