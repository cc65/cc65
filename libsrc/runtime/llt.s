;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare < for long ints
;

        .export         toslteax
        .import         toslcmp, boollt

toslteax:
        jsr     toslcmp         ; Set the flags
        jmp     boollt          ; Convert to boolean
