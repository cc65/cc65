;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare <= for signed ints
;

        .export         tosle00, toslea0, tosleax
        .import         tosicmp, boolle

tosle00:
        lda     #$00
toslea0:
        ldx     #$00
tosleax:
        jsr     tosicmp         ; Set flags
        jmp     boolle          ; Convert to boolean

