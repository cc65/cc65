;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare <= for unsigned ints
;

        .export         tosule00, tosulea0, tosuleax
        .import         tosicmp, boolule

tosule00:
        lda     #$00
tosulea0:
        ldx     #$00
tosuleax:
        jsr     tosicmp         ; Set flags
        jmp     boolule         ; Convert to boolean


