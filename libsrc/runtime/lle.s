;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare <= for long ints
;

        .export         tosleeax
        .import         toslcmp, boolle

tosleeax:
        jsr     toslcmp         ; Set the flags
        jmp     boolle          ; Convert to boolean

