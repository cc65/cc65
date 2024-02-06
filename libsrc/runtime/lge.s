;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare >= for long ints
;

        .export         tosgeeax
        .import         toslcmp, boolge

tosgeeax:
        jsr     toslcmp         ; Set the flags
        jmp     boolge          ; Convert to boolean

