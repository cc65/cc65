;
; Ullrich von Bassewitz, 17.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: division for long unsigned ints
;

        .export         tosudiv0ax, tosudiveax, getlop, udiv32
        .import         addysp1
        .importzp       sp, sreg, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

        .macpack        cpu

tosudiv0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif

tosudiveax:                         
        jsr     getlop          ; Get the paramameters
        jsr     udiv32          ; Do the division
        lda     ptr1            ; Result is in ptr1:sreg
        ldx     ptr1+1
        rts

; Pop the parameters for the long division and put it into the relevant
; memory cells. Called from the signed divisions also.

getlop: sta     ptr3            ; Put right operand in place
        stx     ptr3+1
        lda     sreg
        sta     ptr4
        lda     sreg+1
        sta     ptr4+1

.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (sp)
        ldy     #1
.else
        ldy     #0              ; Put left operand in place
        lda     (sp),y
        iny
.endif
        sta     ptr1
        lda     (sp),y
        sta     ptr1+1
        iny
        lda     (sp),y
        sta     sreg
        iny
        lda     (sp),y
        sta     sreg+1
        jmp     addysp1         ; Drop parameters

; Do (ptr1:sreg) / (ptr3:ptr4) --> (ptr1:sreg), remainder in (ptr2:tmp3:tmp4)
; This is also the entry point for the signed division

udiv32: lda     #0
        sta     ptr2+1
        sta     tmp3
        sta     tmp4
;       sta     ptr1+1
        ldy     #32
L0:     asl     ptr1
        rol     ptr1+1
        rol     sreg
        rol     sreg+1
        rol     a
        rol     ptr2+1
        rol     tmp3
        rol     tmp4

; Do a subtraction. we do not have enough space to store the intermediate
; result, so we may have to do the subtraction twice.

        tax
        cmp     ptr3
        lda     ptr2+1
        sbc     ptr3+1
        lda     tmp3
        sbc     ptr4
        lda     tmp4
        sbc     ptr4+1
        bcc     L1

; Overflow, do the subtraction again, this time store the result

        sta     tmp4            ; We have the high byte already
        txa
        sbc     ptr3            ; byte 0
        tax
        lda     ptr2+1
        sbc     ptr3+1
        sta     ptr2+1          ; byte 1
        lda     tmp3
        sbc     ptr4
        sta     tmp3            ; byte 2
        inc     ptr1            ; Set result bit

L1:     txa
        dey
        bne     L0
        sta     ptr2
        rts


