;
; Piotr Fusik, 15.04.2002
; originally by Ullrich von Bassewitz
;
; Long int compare function - used by the compare operators
;

        .export         toslcmp
        .import         incsp4
        .importzp       sp, sreg, ptr1


toslcmp:
        sta     ptr1
        stx     ptr1+1          ; EAX now in sreg:ptr1

        ldy     #$03
        lda     (sp),y
        sec
        sbc     sreg+1
        bne     L4

        dey
        lda     (sp),y
        cmp     sreg
        bne     L1

        dey
        lda     (sp),y
        cmp     ptr1+1
        bne     L1

        dey
        lda     (sp),y
        cmp     ptr1

L1:     php                     ; Save flags
        jsr     incsp4          ; Drop TOS
        plp                     ; Restore the flags
        beq     L2
        bcs     L3
        lda     #$FF            ; Set the N flag
L2:     rts

L3:     lda     #$01            ; Clear the N flag
        rts

L4:     bvc     L5
        eor     #$FF            ; Fix the N flag if overflow
        ora     #$01            ; Clear the Z flag
L5:     php                     ; Save flags
        jsr     incsp4          ; Drop TOS
        plp                     ; Restore flags
        rts


