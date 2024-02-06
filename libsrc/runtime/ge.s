;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare >= for signed ints
;

        .export         tosge00, tosgea0, tosgeax
        .import         tosicmp, boolge


tosge00:
        lda     #$00
tosgea0:
        ldx     #$00
tosgeax:
        jsr     tosicmp         ; Set flags
        jmp     boolge          ; Convert to boolean
