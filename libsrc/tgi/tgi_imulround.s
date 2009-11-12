;
; Ullrich von Bassewitz, 2009-11-05
;
; Helper function for graphics functions: Multiply two values, one being 
; an 8.8 fixed point one, and return the rounded and scaled result.
;
; The module has two entry points: One is C callable and expects the
; parameters in ax and the stack, the second is assembler callable and
; expects the parameters in ax and ptr1
;


        .export         _tgi_imulround, tgi_imulround
        .import         popax, imul16x16r32

        .include        "zeropage.inc"


;----------------------------------------------------------------------------
;

.code

; C callable entry point
_tgi_imulround:

; Get arguments

        sta     ptr1
        stx     ptr1+1                  ; Save lhs
        jsr     popax                   ; Get rhs

; ASM callable entry point
tgi_imulround:

; Multiplicate

        jsr     imul16x16r32

; Round the result

        cmp     #$80                    ; Frac(x) >= 0.5?
        txa
        ldy     sreg+1                  ; Check sign
        bmi     @L1

        adc     #$00
        tay
        lda     sreg
        adc     #$00
        tax
        tya
        rts

@L1:    sbc     #$00
        tay
        lda     sreg
        sbc     #$00
        tax
        tya
        rts

     
