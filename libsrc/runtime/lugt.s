;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare > for long unsigneds
;

        .export         tosugteax
        .import         toslcmp, boolugt

tosugteax:
        jsr     toslcmp         ; Set the flags
        jmp     boolugt         ; Convert to boolean


