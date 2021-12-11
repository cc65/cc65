;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: or on longs
;

        .export         tosor0ax, tosoreax
        .import         addysp1
        .importzp       sp, sreg, tmp1
                                  
        .macpack        cpu

tosor0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif  

tosoreax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        ora     (sp)
        ldy     #1
.else
        ldy     #0
        ora     (sp),y          ; byte 0
        iny
.endif        
        sta     tmp1
        txa
        ora     (sp),y          ; byte 1
        tax
        iny
        lda     sreg
        ora     (sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        ora     (sp),y          ; byte 3
        sta     sreg+1

        lda     tmp1
        jmp     addysp1

