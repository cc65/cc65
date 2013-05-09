;
; Piotr Fusik, 15.04.2002
; originally by Ullrich von Bassewitz
;
; Integer compare function - used by the compare operators
;

        .export         tosicmp, tosicmp0
        .importzp       sp, sreg


tosicmp0:
        ldx     #$00

tosicmp:
        sta     sreg
        stx     sreg+1          ; Save ax

        ldy     #$00
        lda     (sp),y          ; Get low byte
        tax
        inc     sp              ; 5
        bne     @L1             ; 3
        inc     sp+1            ; (5)
@L1:
        lda     (sp),y          ; Get high byte
        inc     sp              ; 5
        bne     @L2             ; 3
        inc     sp+1            ; (5)

; Do the compare.

@L2:    sec
        sbc     sreg+1          ; Compare high byte
        bne     @L4
        cpx     sreg            ; Compare low byte
        beq     @L3
        adc     #$FF            ; If the C flag is set then clear the N flag
        ora     #$01            ; else set the N flag
@L3:    rts

@L4:    bvc     @L3
        eor     #$FF            ; Fix the N flag if overflow
        ora     #$01            ; Clear the Z flag
        rts


