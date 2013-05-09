;
; Ullrich von Bassewitz, 06.08.1998
;
; void clrscr (void);
;

        .export         _clrscr

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   _clrscr
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     CLRSCR                  ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc






