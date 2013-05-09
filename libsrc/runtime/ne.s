;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare != for ints
;

        .export         tosne00, tosnea0, tosneax
        .import         tosicmp, boolne

tosne00:
        lda     #$00
tosnea0:
        ldx     #$00
tosneax:
        jsr     tosicmp         ; Set flags
        jmp     boolne          ; Convert to boolean


