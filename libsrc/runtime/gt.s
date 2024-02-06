;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare > for signed ints
;

        .export         tosgt00, tosgta0, tosgtax
        .import         tosicmp, boolgt


tosgt00:
        lda     #$00
tosgta0:
        ldx     #$00
tosgtax:
        jsr     tosicmp         ; Set the flags
        jmp     boolgt          ; Convert to boolean

