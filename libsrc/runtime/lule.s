;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare <= for long unsigneds
;

        .export         tosuleeax
        .import         toslcmp, boolule

tosuleeax:
        jsr     toslcmp         ; Set the flags
        jmp     boolule         ; Convert to boolean

