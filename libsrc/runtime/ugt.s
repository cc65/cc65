;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare > for unsigned ints
;

        .export         tosugt00, tosugta0, tosugtax
        .import         tosicmp, boolugt


tosugt00:
        lda     #$00
tosugta0:
        ldx     #$00
tosugtax:
        jsr     tosicmp         ; Set flags
        jmp     boolugt         ; Convert to boolean

