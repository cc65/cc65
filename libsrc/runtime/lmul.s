;
; Ullrich von Bassewitz, 13.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: multiplication for long (unsigned) ints
;

        .export         tosumul0ax, tosumuleax, tosmul0ax, tosmuleax
        .import         addysp1
        .importzp       sp, sreg, tmp1, tmp2, tmp3, tmp4, ptr1, ptr3, ptr4

        .macpack        cpu

tosmul0ax:
tosumul0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00    
        sty     sreg
        sty     sreg+1
.endif        

tosmuleax:
tosumuleax:
mul32:  sta     ptr1
        stx     ptr1+1          ; op2 now in ptr1/sreg
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (sp)
        ldy     #1
.else        
        ldy     #0
        lda     (sp),y
        iny
.endif
        sta     ptr3
        lda     (sp),y
        sta     ptr3+1
        iny
        lda     (sp),y
        sta     ptr4
        iny
        lda     (sp),y
        sta     ptr4+1          ; op1 in pre3/ptr4
        jsr     addysp1         ; Drop TOS

; Do (ptr1:sreg)*(ptr3:ptr4) --> EAX.

        lda     #0
        sta     tmp4
        sta     tmp3
        sta     tmp2
        ldy     #32
L0:     lsr     tmp4
        ror     tmp3
        ror     tmp2
        ror     a
        ror     sreg+1
        ror     sreg
        ror     ptr1+1
        ror     ptr1
        bcc     L1
        clc
        adc     ptr3
        tax
        lda     ptr3+1
        adc     tmp2
        sta     tmp2
        lda     ptr4
        adc     tmp3
        sta     tmp3
        lda     ptr4+1
        adc     tmp4
        sta     tmp4
        txa
L1:     dey
        bpl     L0
        lda     ptr1            ; Load the low result word
        ldx     ptr1+1
        rts

