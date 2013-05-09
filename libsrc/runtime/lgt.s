;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare > for long ints
;

        .export         tosgteax
        .import         toslcmp, boolgt

tosgteax:
        jsr     toslcmp         ; Set the flags
        jmp     boolgt          ; Convert to boolean


