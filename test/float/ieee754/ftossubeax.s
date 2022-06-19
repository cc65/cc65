
         .importzp       sp, sreg, tmp1
         .import         addysp1
         .import         addysp
         .import         popax

    .include "ieee754.inc"

    .import ftosaddeax
    
    .export ftossubeax
ftossubeax:
    ; FIXME
    rts 

; found at https://github.com/CrashAndSideburns/6502ieee754/blob/main/arithmetic/addition.s
; subtraction:

  ; Pull MSB of second parameter off of stack, flip sign bit, and push back to stack.
  ; Proceed to addition.
;   TSX
;   TXA
;   CLC
;   ADC #$05
;   TAX
;   TXS
;   PLA
;   EOR #$80
;   PHA
;   TXA
;   SEC
;   SBC #$05
;   TAX
;   TXS

    pha
    ldy     #3
    lda     (sp),y          ; msb
    eor     #$80
    sta     (sp),y          ; msb
    pla

    jmp ftosaddeax
