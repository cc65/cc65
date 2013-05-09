;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long sub reversed
;

;
; EAX = EAX - TOS
;
        .export         tosrsub0ax, tosrsubeax
        .import         addysp1
        .importzp       sp, sreg, tmp1

tosrsub0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tosrsubeax:                         
        ldy     #0
        sec
        sbc     (sp),y          ; byte 0
        sta     tmp1            ; use as temp storage
        txa
        iny
        sbc     (sp),y          ; byte 1
        tax
        iny
        lda     sreg
        sbc     (sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        sbc     (sp),y          ; byte 3
        sta     sreg+1
        lda     tmp1
        jmp     addysp1         ; drop TOS

