;
; Ullrich von Bassewitz, 22.11.2002
;
; CLOSE replacement function
;

        .export         CLOSE

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   CLOSE
        sta     ENABLE_ROM              ; Enable the ROM
        clc                             ; Force C64 compatible behaviour
        jsr     $FFC3                   ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc


