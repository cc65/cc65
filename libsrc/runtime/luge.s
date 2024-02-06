;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare >= for long unsigneds
;

        .export         tosugeeax
        .import         toslcmp, booluge

tosugeeax:
        jsr     toslcmp         ; Set the flags
        jmp     booluge         ; Convert to boolean

