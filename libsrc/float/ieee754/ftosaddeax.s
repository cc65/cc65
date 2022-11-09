
    .include "ieee754.inc"

         .importzp       sp, sreg, tmp1
         .import         addysp1
         .import         addysp
         .import         popax

; Primary = TOS + Primary (like tosaddeax)
    .export ftosaddeax
;ftosaddeax:
    ; FIXME
;    rts

; CC65 runtime: long add

; ; EAX = TOS + EAX
;
; ;tosadd0ax:
;         ldy     #$00
;         sty     sreg
;         sty     sreg+1
;
; ;tosaddeax:
;         clc
;         ldy     #0
;         adc     (sp),y          ; lo byte
;         iny
;
;         sta     tmp1            ; use as temp storage
;         txa
;         adc     (sp),y          ; byte 1
;         tax
;         iny
;
;         lda     sreg
;         adc     (sp),y          ; byte 2
;         sta     sreg
;         iny
;
;         lda     sreg+1
;         adc     (sp),y          ; byte 3
;         sta     sreg+1
;         lda     tmp1            ; load byte 0
;         jmp     addysp1         ; drop TOS


; found at https://github.com/CrashAndSideburns/6502ieee754/blob/main/arithmetic/addition.s

;addition:
ftosaddeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)

    lda #0
    sta SIGN_ONE
    sta SIGN_TWO

  ; Pull 2 LSBs of second parameter off of stack.
    pha
    lda sreg+1
    ASL
    STA MANTISSA_TWO + 3
    lda sreg+0
    ROL
    STA MANTISSA_TWO + 2

    ; Pull MSB of mantissa off of stack and shift exponent LSB into carry.
    tax
    ROL
    STA MANTISSA_TWO + 1

    ; Pull MSB off of stack, shift in exponent LSB, store sign.
    PLA
    ROL
    STA EXPONENT_TWO
    ROL SIGN_TWO

    ;-----------------

  ; Pull 2 LSBs of first parameter off of stack.
    ldy     #0
    lda     (sp),y          ; lsb
    ASL
    STA MANTISSA_ONE + 3
    iny
    lda     (sp),y
    ROL
    STA MANTISSA_ONE + 2

  ; Pull MSB of mantissa off of stack and shift exponent LSB into carry.
    iny
    lda     (sp),y
    ROL
    STA MANTISSA_ONE + 1

  ; Pull MSB off of stack, shift in exponent LSB, store sign.
    iny
    lda     (sp),y          ; msb
    ROL
    STA EXPONENT_ONE
    ROL SIGN_ONE


  ; Add implicit 1 to mantissas if appropriate.
@add_second_implicit_bit:
  LDA EXPONENT_TWO
  CMP #$00
  BEQ @add_first_implicit_bit
  SEC
  ROR MANTISSA_TWO + 1
  ROR MANTISSA_TWO + 2
  ROR MANTISSA_TWO + 3

@add_first_implicit_bit:
  LDA EXPONENT_ONE
  CMP #$00
  BEQ @manage_special_values
  SEC
  ROR MANTISSA_ONE + 1
  ROR MANTISSA_ONE + 2
  ROR MANTISSA_ONE + 3

@manage_special_values:
  ; Check if first parameter has exponent #$ff.
  ; If it does, first parameter is some special quantity.
  ; If not, check if second parameter has epxonent #$ff.
  ; If it does, then it is special and the first parameter is not, so propagate.
  ; If neither parameter is special, move on to aligning mantissas.
  LDA EXPONENT_ONE
  CMP #$ff
  BEQ @first_parameter_special
  LDA EXPONENT_TWO
  CMP #$ff
  BEQ @propagate_second_parameter
  JMP @align_mantissas

@first_parameter_special:
  ; Exponent is non-zero, so an implicit 1 has been added to mantissa.
  ; Unless the mantissa with added implicit 1 is $800000, return first param.
  ; If the mantissa is precisely $800000, first param is ±∞.
  LDA MANTISSA_ONE + 1
  CMP #$80
  BNE @propagate_first_parameter
  LDA MANTISSA_ONE + 2
  CMP #$00
  BNE @propagate_first_parameter
  LDA MANTISSA_ONE + 3
  CMP #$00
  BEQ @first_parameter_infinite

@propagate_first_parameter:
  ; Shift out implicit bit, shift exponent and sign through.
  ; Push return value onto stack.
  ; Push return address onto stack.
  ; Return from subroutine.
  ASL MANTISSA_ONE + 1
  LSR SIGN_ONE
  ROR EXPONENT_ONE
  ROR MANTISSA_ONE + 1

;   LDA EXPONENT_ONE
;   PHA
;   LDA MANTISSA_ONE + 1
;   PHA
;   LDA MANTISSA_ONE + 2
;   PHA
;   LDA MANTISSA_ONE + 3
;   PHA

;   LDA $01
;   PHA
;   LDA $00
;   PHA

  LDA EXPONENT_ONE
  sta sreg+1
  LDA MANTISSA_ONE + 1
  sta sreg+0
  LDX MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 3
;   ldx #$43
;   lda #$21
  jmp     addysp1         ; drop TOS

  RTS

@first_parameter_infinite:
  ; Check if second parameter is also ±∞.
  ; If second param is not special, propagate infinity.
  ; If second param is NaN, propagate NaN.
  LDA EXPONENT_TWO
  CMP #$ff
  BNE @propagate_first_parameter
  LDA MANTISSA_TWO + 1
  CMP #$80
  BNE @propagate_second_parameter
  LDA MANTISSA_TWO + 2
  CMP #$00
  BNE @propagate_second_parameter
  LDA MANTISSA_TWO + 3
  CMP #$00
  BNE @propagate_second_parameter

@adding_infinities:
  ; First and second parameters are both ±∞.
  ; Check if they have opposite sign.
  ; (+∞)+(-∞) = (-∞)+(+∞) = NaN.
  ; If they have opposite sign, return a NaN.
  ; Otherwise, return the first parameter.
  LDA SIGN_ONE
  EOR SIGN_TWO
  BEQ @propagate_first_parameter
;   LDA #$ff
;   PHA
;   PHA
;   PHA
;   PHA

  LDA #$ff
  sta sreg+1
  sta sreg
  tax
  jmp     addysp1         ; drop TOS

;   LDA $01
;   PHA
;   LDA $00
;   PHA

  RTS

@propagate_second_parameter:
  ; Shift out implicit bit, shift exponent and sign through.
  ; Push return value onto stack.
  ; Push return address onto stack.
  ; Return from subroutine.
  ASL MANTISSA_TWO + 1
  LSR SIGN_TWO
  ROR EXPONENT_TWO
  ROR MANTISSA_TWO + 1

;   LDA EXPONENT_TWO
;   PHA
;   LDA MANTISSA_TWO + 1
;   PHA
;   LDA MANTISSA_TWO + 2
;   PHA
;   LDA MANTISSA_TWO + 3
;   PHA

;   LDA $01
;   PHA
;   LDA $00
;   PHA

  LDA EXPONENT_TWO
  sta sreg+1
  LDA MANTISSA_TWO + 1
  sta sreg
  LDX MANTISSA_TWO + 2
  LDA MANTISSA_TWO + 3
  jmp     addysp1         ; drop TOS

  RTS

@align_mantissas:
  ; Check if the mantissas differ by 25 or more.
  ; If they do, propagate the parameter with the larger mantissa.
  ; If they don't, shift the mantissa of the smaller parameter right.
  SEC
  LDA EXPONENT_TWO
  SBC EXPONENT_ONE
  BCC @first_difference_underflow
  CMP #25
  BCS @propagate_second_parameter
@first_difference_underflow:
  SEC
  LDA EXPONENT_ONE
  SBC EXPONENT_TWO
  BCC @second_difference_underflow
  CMP #25
  BCS @propagate_first_parameter
@second_difference_underflow:
  CMP #$00
  BEQ @apply_signs
  TAX
  BPL @shift_second_mantissa
@shift_first_mantissa:
  LSR MANTISSA_ONE + 1
  ROR MANTISSA_ONE + 2
  ROR MANTISSA_ONE + 3
  ROR MANTISSA_ONE + 4
  ROR MANTISSA_ONE + 5
  ROR MANTISSA_ONE + 6
  INX
  CPX #$00
  BNE @shift_first_mantissa

  LDA EXPONENT_TWO
  STA EXPONENT_ONE
  JMP @apply_signs
@shift_second_mantissa:
  LSR MANTISSA_TWO + 1
  ROR MANTISSA_TWO + 2
  ROR MANTISSA_TWO + 3
  ROR MANTISSA_TWO + 4
  ROR MANTISSA_TWO + 5
  ROR MANTISSA_TWO + 6
  DEX
  CPX #$00
  BNE @shift_second_mantissa

@apply_signs:
  ; Check the signs of both floats.
  ; If a float has a 1 sign bit, take the 2's complement of the mantissa.
@negate_first_mantissa:
  LDA SIGN_ONE
  CMP #$00
  BEQ @negate_second_mantissa
  LDA MANTISSA_ONE
  EOR #$ff
  STA MANTISSA_ONE
  LDA MANTISSA_ONE + 1
  EOR #$ff
  STA MANTISSA_ONE + 1
  LDA MANTISSA_ONE + 2
  EOR #$ff
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 3
  EOR #$ff
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 4
  EOR #$ff
  STA MANTISSA_ONE + 4
  LDA MANTISSA_ONE + 5
  EOR #$ff
  STA MANTISSA_ONE + 5
  LDA MANTISSA_ONE + 6
  EOR #$ff
  CLC
  ADC #$01
  STA MANTISSA_ONE + 6
  LDA MANTISSA_ONE + 5
  ADC #$00
  STA MANTISSA_ONE + 5
  LDA MANTISSA_ONE + 4
  ADC #$00
  STA MANTISSA_ONE + 4
  LDA MANTISSA_ONE + 3
  ADC #$00
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 2
  ADC #$00
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 1
  ADC #$00
  STA MANTISSA_ONE + 1
  LDA MANTISSA_ONE
  ADC #$00
  STA MANTISSA_ONE
@negate_second_mantissa:
  LDA SIGN_TWO
  CMP #$00
  BEQ @sum_mantissas
  LDA MANTISSA_TWO
  EOR #$ff
  STA MANTISSA_TWO
  LDA MANTISSA_TWO + 1
  EOR #$ff
  STA MANTISSA_TWO + 1
  LDA MANTISSA_TWO + 2
  EOR #$ff
  STA MANTISSA_TWO + 2
  LDA MANTISSA_TWO + 3
  EOR #$ff
  STA MANTISSA_TWO + 3
  LDA MANTISSA_TWO + 4
  EOR #$ff
  STA MANTISSA_TWO + 4
  LDA MANTISSA_TWO + 5
  EOR #$ff
  STA MANTISSA_TWO + 5
  LDA MANTISSA_TWO + 6
  EOR #$ff
  CLC
  ADC #$01
  STA MANTISSA_TWO + 6
  LDA MANTISSA_TWO + 5
  ADC #$00
  STA MANTISSA_TWO + 5
  LDA MANTISSA_TWO + 4
  ADC #$00
  STA MANTISSA_TWO + 4
  LDA MANTISSA_TWO + 3
  ADC #$00
  STA MANTISSA_TWO + 3
  LDA MANTISSA_TWO + 2
  ADC #$00
  STA MANTISSA_TWO + 2
  LDA MANTISSA_TWO + 1
  ADC #$00
  STA MANTISSA_TWO + 1
  LDA MANTISSA_TWO
  ADC #$00
  STA MANTISSA_TWO

@sum_mantissas:
  ; Sum the mantissas to obtain the mantissa of the return float.
  ; Check if the resultant mantissa has a 1 MSB.
  ; If it does, set the result sign to 1 and take the 2's complement of the mantissa.
  ; If it doesn't, move on to normalising the resultant mantissa.
  CLC
  LDA MANTISSA_ONE + 6
  ADC MANTISSA_TWO + 6
  STA MANTISSA_ONE + 6
  LDA MANTISSA_ONE + 5
  ADC MANTISSA_TWO + 5
  STA MANTISSA_ONE + 5
  LDA MANTISSA_ONE + 4
  ADC MANTISSA_TWO + 4
  STA MANTISSA_ONE + 4
  LDA MANTISSA_ONE + 3
  ADC MANTISSA_TWO + 3
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 2
  ADC MANTISSA_TWO + 2
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 1
  ADC MANTISSA_TWO + 1
  STA MANTISSA_ONE + 1
  LDA MANTISSA_ONE
  ADC MANTISSA_TWO
  STA MANTISSA_ONE

  AND #$80
  CMP #$00
  BEQ @positive_sum

  LDA MANTISSA_ONE
  EOR #$ff
  STA MANTISSA_ONE
  LDA MANTISSA_ONE + 1
  EOR #$ff
  STA MANTISSA_ONE + 1
  LDA MANTISSA_ONE + 2
  EOR #$ff
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 3
  EOR #$ff
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 4
  EOR #$ff
  STA MANTISSA_ONE + 4
  LDA MANTISSA_ONE + 5
  EOR #$ff
  STA MANTISSA_ONE + 5
  LDA MANTISSA_ONE + 6
  EOR #$ff
  CLC
  ADC #$01
  STA MANTISSA_ONE + 6
  LDA MANTISSA_ONE + 5
  ADC #$00
  STA MANTISSA_ONE + 5
  LDA MANTISSA_ONE + 4
  ADC #$00
  STA MANTISSA_ONE + 4
  LDA MANTISSA_ONE + 3
  ADC #$00
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 2
  ADC #$00
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 1
  ADC #$00
  STA MANTISSA_ONE + 1
  LDA MANTISSA_ONE
  ADC #$00
  STA MANTISSA_ONE
  LDA #$01
  STA SIGN_ONE
  JMP @normalise_mantissa
@positive_sum:
  LDA #$00
  STA SIGN_ONE

@normalise_mantissa:
  ; Now that the new mantissa has been computed, normalise it.
  ; Check if the LSB of the byte before the MSB of the mantissa is 1.
  ; If it is, shift the mantissa down and increment the mantissa.
  ; If it isn't, move on to checking if we must shift left.
  LDA MANTISSA_ONE
  AND #$01
  CMP #$01
  BNE @no_overflow
  LSR MANTISSA_ONE
  ROR MANTISSA_ONE + 1
  ROR MANTISSA_ONE + 2
  ROR MANTISSA_ONE + 3
  ROR MANTISSA_ONE + 4
  ROR MANTISSA_ONE + 5
  ROR MANTISSA_ONE + 6
  INC EXPONENT_ONE
  JMP @round

@no_overflow:
  ; Check if the MSB of the mantissa is 1.
  ; If it is, move on to rounding.
  ; If it isn't, shift mantissa left and decrement exponent until MSB is 1 or exponent is 0.
  LDA MANTISSA_ONE + 1
  AND #$80
  CMP #$80
  BEQ @round
  LDA EXPONENT_ONE
  CMP #$00
  BEQ @round
  ASL MANTISSA_ONE + 6
  ROL MANTISSA_ONE + 5
  ROL MANTISSA_ONE + 4
  ROL MANTISSA_ONE + 3
  ROL MANTISSA_ONE + 2
  ROL MANTISSA_ONE + 1
  ROL MANTISSA_ONE
  DEC EXPONENT_ONE
  JMP @no_overflow

@round:
  ; Check if we have already produced ±∞ in the event that we decide not to round.
  ; If we have, return ±∞.
  ; If we haven't, check if the MSB of the byte after the mantissa is 0.
  ; If it is, return, trucating the subsequent bits.
  ; If it isn't, check if any subsequent bits are 1.
  ; If any are, round up.
  ; If none are, check if the LSB of the mantissa is 1.
  ; If it is, round up.
  ; If it isn't, return.
  LDA EXPONENT_ONE
  CMP #$ff
  BEQ @return_infinity
  LDA MANTISSA_ONE + 4
  AND #$80
  CMP #$00
  BEQ @return
  LDA MANTISSA_ONE + 4
  AND #$7f
  CMP #$00
  BNE @round_up
  LDA MANTISSA_ONE + 5
  CMP #$00
  BNE @round_up
  LDA MANTISSA_ONE + 6
  CMP #$00
  BNE @round_up
  LDA MANTISSA_ONE + 3
  AND #$01
  CMP #$00
  BEQ @return

@round_up:
  ; Increment the mantissa by 1.
  ; If the increment doesn't overflow, return.
  ; If it does, shift the mantissa right and increment the exponent.
  ; Check if incrementing the exponent left us with an exponent of #$ff.
  ; If it did, return ±∞.
  ; If it didn't, round again.
  CLC
  LDA MANTISSA_ONE + 3
  ADC #$01
  STA MANTISSA_ONE + 3
  LDA MANTISSA_ONE + 2
  ADC #$00
  STA MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 1
  ADC #$00
  STA MANTISSA_ONE + 1
  BCC @return
  ROL MANTISSA_ONE + 1
  ROL MANTISSA_ONE + 2
  ROL MANTISSA_ONE + 3
  ROL MANTISSA_ONE + 4
  ROL MANTISSA_ONE + 5
  ROL MANTISSA_ONE + 6
  INC EXPONENT_ONE
  LDA EXPONENT_ONE
  CMP #$ff
  BEQ @return_infinity
  JMP @round

@return_infinity:
  ; If we have to return ±∞ due to an over/underflow, clear the mantissa.
  LDA #$00
  STA MANTISSA_ONE + 1
  STA MANTISSA_ONE + 2
  STA MANTISSA_ONE + 3
@return:
  ; Shift out implicit bit, shift exponent and sign through.
  ; Push return value onto stack.
  ; Push return address onto stack.
  ; Return from subroutine.
  LDA EXPONENT_ONE
  CMP #$00
  BNE @return_normal
  LSR MANTISSA_ONE + 1
  ROR MANTISSA_ONE + 2
  ROR MANTISSA_ONE + 3
  BCS @round_up
@return_normal:
  ASL MANTISSA_ONE + 1
  LSR SIGN_ONE
  ROR EXPONENT_ONE
  ROR MANTISSA_ONE + 1

;   LDA EXPONENT_ONE
;   PHA
;   LDA MANTISSA_ONE + 1
;   PHA
;   LDA MANTISSA_ONE + 2
;   PHA
;   LDA MANTISSA_ONE + 3
;   PHA

;   LDA $01
;   PHA
;   LDA $00
;   PHA

  LDA EXPONENT_ONE
  sta sreg+1
  LDA MANTISSA_ONE + 1
  sta sreg
  LDX MANTISSA_ONE + 2
  LDA MANTISSA_ONE + 3
  jmp     addysp1         ; drop TOS

  RTS

