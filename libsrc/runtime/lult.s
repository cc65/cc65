;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare < for long unsigneds
;

        .export         tosulteax
        .import         toslcmp, boolult

tosulteax:
        jsr     toslcmp         ; Set the flags
        jmp     boolult         ; Convert to boolean
