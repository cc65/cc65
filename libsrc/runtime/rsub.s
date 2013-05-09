;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: sub ints reversed
;

        .export         tosrsuba0, tosrsubax
        .import         addysp1
        .importzp       sp, tmp1

;
; AX = AX - TOS
;

tosrsuba0:
        ldx     #0
tosrsubax:
        ldy     #0
        sec
        sbc     (sp),y          ; lo byte
        sta     tmp1            ; save lo byte
        txa
        iny
        sbc     (sp),y          ; hi byte
        tax
        lda     tmp1
        jmp     addysp1         ; drop TOS, set condition codes


