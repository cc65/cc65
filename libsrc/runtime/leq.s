;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long equal
;

        .export         toseqeax
        .import         toslcmp, booleq

toseqeax:                       
        jsr     toslcmp         ; Set flags
        jmp     booleq          ; Convert to boolean



