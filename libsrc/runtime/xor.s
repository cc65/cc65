;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: xor on ints
;

        .export         tosxora0, tosxorax
        .import         addysp1
        .importzp       sp, tmp1

tosxora0:
        ldx     #$00
tosxorax:
        ldy     #0
        eor     (sp),y
        sta     tmp1
        iny
        txa
        eor     (sp),y
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes

