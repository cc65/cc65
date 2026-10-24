; Opcodes with Immediate Implicit vs Immediate Accumulator addressing.
; Switching CPUs must not break operand sizes.

.smart -                ; Stop being smart

.setcpu "65C02"
    adc     #$12        ; Immediate Implicit
    ora     #$34
    cmp     #$12
    lda     #$34
    cpx     #$12
    ldy     #$34
    bit     #$56
    bit     $CD         ; Direct
    brk     $FF         ; Direct
    brk     #$FF        ; Immediate Implicit
    brk                 ; Implicit

.setcpu "65816"
.a8
.i8
    adc     #$12        ; Immediate Accumulator
    ora     #$34
    cmp     #$12
    lda     #$34
    cpx     #$12        ; Immediate Index Reg
    ldy     #$34
    bit     #$56        ; Immediate Accumulator
    bit     $CD         ; Direct
    brk     $FF         ; Direct
    rep     #$20        ; Immediate Implicit
    sep     #$10
    brk     #$FF
    brk                 ; Implicit

.a16
.i16
    adc     #$1234      ; Immediate Accumulator
    ora     #$5678
    cmp     #$1234
    lda     #$5678
    cpx     #$1234      ; Immediate Index Reg
    ldy     #$5678
    bit     #$ABCD      ; Immediate Accumulator
    bit     $CDEF       ; Absolute
    brk     $FF         ; Direct
    rep     #$20        ; Immediate Implicit
    sep     #$10
    brk     #$FF
    brk                 ; Implicit

; Switch back to 65C02 must keep immediate operands at 8-bit.

.setcpu "65C02"
    adc     #$12        ; Immediate Implicit
    ora     #$34
    cmp     #$12
    lda     #$34
    cpx     #$12
    ldy     #$34
    bit     #$56
    bit     $CD         ; Direct
    brk     $FF         ; Direct
    brk     #$FF        ; Immediate Implicit
    brk                 ; Implicit


; Return to 65816 keeps prior operand sizes (.asize and .isize)!
; Test 65816 smart mode as well.

.smart +                ; Resume smartarsery

.setcpu "65816"

    ; Now in .a16/.i16 mode
    lda     #$1234      ; Immediate Accumulator
    eor     #$5678
    cpa     #$1234
    cpy     #$5678      ; Immediate Index Reg
    ldx     #$1234
    bit     #$5678      ; Immediate Accumulator

    sep     #$20        ; Immediate Implicit
    ; Now in .a8/.i16 mode
    lda     #$12        ; Immediate Accumulator
    sbc     #$34
    and     #$12
    cpy     #$5678      ; Immediate Index Reg
    ldx     #$1234
    bit     #$56        ; Immediate Accumulator

    rep     #$20        ; Immediate Implicit
    sep     #$10        ; Immediate Implicit
    ; Now in .a16/.i8 mode
    lda     #$1234      ; Immediate Accumulator
    sbc     #$5678
    and     #$1234
    cpy     #$12        ; Immediate Index Reg
    ldx     #$34
    bit     #$5678      ; Immediate Accumulator

    brk     $FF         ; Direct
    brk     #$FF        ; Immediate Implicit
    brk                 ; Implicit

; Again, switch back to 65C02 must keep immediate operands at 8-bit.

.setcpu "65C02"
    lda     #$12        ; Immediate Implicit
    eor     #$34
    and     #$12
    cmp     #$34
    cpy     #$12
    ldx     #$34
    bit     #$56
    bit     $CD         ; Direct
    brk     $FF         ; Direct
    brk     #$FF        ; Immediate Implicit
    brk                 ; Implicit
