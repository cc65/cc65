;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: or on ints
;

        .export         tosora0, tosorax
        .import         addysp1
        .importzp       sp, tmp1

tosora0:
        ldx     #$00
tosorax:
        ldy     #0
        ora     (sp),y
        sta     tmp1
        iny
        txa
        ora     (sp),y
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes

