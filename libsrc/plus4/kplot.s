;
; Ullrich von Bassewitz, 22.11.2002
;
; PLOT replacement function
;

        .export         PLOT

.scope  KERNAL
        .include        "cbm_kernal.inc"
.endscope

        .include        "plus4.inc"

.segment        "LOWCODE"               ; Must go into low memory

.proc   PLOT
        sta     ENABLE_ROM              ; Enable the ROM
        jsr     KERNAL::PLOT            ; Call the ROM routine
        sta     ENABLE_RAM              ; Switch back to RAM
        rts                             ; Return to caller
.endproc
