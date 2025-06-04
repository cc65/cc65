;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Compare == for ints
;

        .export         toseq00, toseqa0, toseqax
        .import         tosicmp, booleq
        .importzp       c_sp, tmp1

toseq00:
        lda     #$00
toseqa0:
        ldx     #$00
toseqax:
        jsr     tosicmp         ; Set flags
        jmp     booleq          ; Convert to boolean
