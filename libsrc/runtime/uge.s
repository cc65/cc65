;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare >= for unsigned ints
;

        .export         tosuge00, tosugea0, tosugeax
        .import         tosicmp, booluge


tosuge00:
        lda     #$00
tosugea0:
        ldx     #$00
tosugeax:
        jsr     tosicmp         ; Set flags
        jmp     booluge         ; Convert to boolean



