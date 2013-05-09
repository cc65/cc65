;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long not equal
;

        .export         tosneeax
        .import         toslcmp, boolne

tosneeax:
        jsr     toslcmp         ; Set flags
        jmp     boolne          ; Convert to boolean


