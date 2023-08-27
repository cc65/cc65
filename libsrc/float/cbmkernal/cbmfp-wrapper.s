
    .import __ctof
    .import __utof
    .import __itof
    .import __stof

; 8bit signed -> float
    .export ___cbmkernal_afloat
___cbmkernal_afloat:
    jmp __ctof

; 8bit unsigned -> float
    .export ___cbmkernal_aufloat
___cbmkernal_aufloat:
    jmp __utof

; 16bit signed -> float
    .export ___cbmkernal_axfloat
___cbmkernal_axfloat:
    jmp __itof

; 16bit unsigned -> float
    .export ___cbmkernal_axufloat
___cbmkernal_axufloat:
    jmp __stof

; FIXME: this might be more accurate when done directly in one step (but the
;        kernal can not do this for 32bit)
; 32bit signed -> float
    .importzp sreg, tmp1
    .import pusheax

    .export ___cbmkernal_eaxfloat
___cbmkernal_eaxfloat:

    sta tmp1

    lda sreg
    pha
    lda sreg+1
    pha

    ; get lower 16bit
    lda tmp1

    ; convert lower 16 bit
    jsr __stof
    jsr pusheax     ; push eax to stack

    ; / 65536
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1

    jsr __fdiv
    jsr pusheax     ; push eax to stack

    ; get higher 16bit
    pla
    tax
    pla
    ; convert higher 16 bit
    jsr __itof

    jsr __fadd

    jsr pusheax     ; push eax to stack

    ; * 65536
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1

    jmp __fmul

; FIXME: this might be more accurate when done directly in one step (but the
;        kernal can not do this for 32bit)
; 32bit unsigned -> float
    .importzp sreg, tmp1
    .import pusheax

    .export ___cbmkernal_eaxufloat
___cbmkernal_eaxufloat:

    sta tmp1

    lda sreg
    pha
    lda sreg+1
    pha

    ; get lower 16bit
    lda tmp1

    ; convert lower 16 bit
    jsr __stof
    jsr pusheax     ; push eax to stack

    ; / 65536
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1

    jsr __fdiv
    jsr pusheax     ; push eax to stack

    ; get higher 16bit
    pla
    tax
    pla
    ; convert higher 16 bit
    jsr __stof

    jsr __fadd

    jsr pusheax     ; push eax to stack

    ; * 65536
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1

    jmp __fmul

;--------------------------------------------------------------

    .import __ftoi

; float -> 16bit int
    .export ___cbmkernal_feaxint
___cbmkernal_feaxint:
    jmp __ftoi

; FIXME: this might be more accurate when done directly in one step (but the
;        kernal can not do this for 32bit)
; float -> 32bit int
    .importzp tmp1, tmp2
    .export ___cbmkernal_feaxlong
___cbmkernal_feaxlong:

    jsr pusheax

    ; primary = primary / 65536
    jsr pusheax     ; push eax to stack
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1
    jsr __fdiv      ; primary / TOS
    ; convert result to int and save
    jsr __ftoi
    sta tmp1
    stx tmp2
    ; convert back to float
    jsr __stof
    ; primary = primary * 65536
    jsr pusheax     ; push eax to stack
    ldx #$00
    lda #$80
    sta sreg
    lda #$47
    sta sreg+1
    jsr __fmul      ; primary * TOS
    ; substract the result from the total number to get the rest
    jsr __fsub
    ; convert rest to int
    jsr __ftoi

    ldy tmp2
    sty sreg+1
    ldy tmp1
    sty sreg
    rts

;--------------------------------------------------------------
    .import __fnot
    .import __fneg

; binary negate (not) for the ! operator. returns a bool!
    .export ___cbmkernal_fbnegeax
    .import bnegax
___cbmkernal_fbnegeax:
; FIXME: why does this not work with __fnot?
    jsr __ftoi
    jmp bnegax

   .export ___cbmkernal_fnegeax
___cbmkernal_fnegeax:
    jmp __fneg

;--------------------------------------------------------------
; math ops

    .import __fadd
    .import __fsub
    .import __fdiv
    .import __fmul

; Primary = TOS + Primary (like tosaddeax)
    .export ___cbmkernal_ftosaddeax
___cbmkernal_ftosaddeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)

    ; FAC: ieee float is in A/X/sreg/sreg+1
    jmp     __fadd

    .export ___cbmkernal_ftossubeax
___cbmkernal_ftossubeax:
    jmp     __fsub
    .export ___cbmkernal_ftosdiveax
___cbmkernal_ftosdiveax:
    jmp     __fdiv
    .export ___cbmkernal_ftosmuleax
___cbmkernal_ftosmuleax:
    jmp     __fmul

;--------------------------------------------------------------

    .import __fcmp

    ; test for equal
    .export ___cbmkernal_ftoseqeax
___cbmkernal_ftoseqeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #0
    beq @equal
    lda #0
    tax
    rts
@equal:
    ldx #0
    lda #1
    rts

    ; test for not equal
    .export ___cbmkernal_ftosneeax
___cbmkernal_ftosneeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #0
    beq @equal
    ldx #0
    lda #1
    rts
@equal:
    lda #0
    tax
    rts

    ; Test for greater than
    .export ___cbmkernal_ftosgteax
___cbmkernal_ftosgteax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #255
    bne @biggerorequal
    ; less
    ldx #0
    lda #1
    rts
@biggerorequal:
    lda #0
    tax
    rts

    ; Test for less than
    .export ___cbmkernal_ftoslteax
___cbmkernal_ftoslteax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #1
    beq @bigger
    lda #0
    tax
    rts
@bigger:
    ldx #0
    lda #1
    rts

    ; Test for greater than or equal to
    .export ___cbmkernal_ftosgeeax
___cbmkernal_ftosgeeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #1
    beq @bigger
    ; less than or equal
    ldx #0
    lda #1
    rts
@bigger:
    lda #0
    tax
    rts

    ; Test for less than or equal to
    .export ___cbmkernal_ftosleeax
___cbmkernal_ftosleeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr     __fcmp
    ; a=0 (==) / a=1 (>) / a=255 (<)
    cmp #255
    beq @smaller
    ; greater than or equal
    ldx #0
    lda #1
    rts
@smaller:
    lda #0
    tax
    rts

;--------------------------------------------------------------
; math.h

    .import __fpow
    .import __fsin
    .import __fcos
    .import __flog
    .import __fexp
    .import __fsqr
    .import __ftan
    .import __fatn
    .import __fabs
    .import __fround
    .import __fint

    .export ___cbmkernal_powf
___cbmkernal_powf:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jmp     __fpow

    .export ___cbmkernal_sinf
___cbmkernal_sinf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fsin

    .export ___cbmkernal_cosf
___cbmkernal_cosf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fcos

    .export ___cbmkernal_logf
___cbmkernal_logf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __flog

    .export ___cbmkernal_expf
___cbmkernal_expf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fexp

    .export ___cbmkernal_sqrtf
___cbmkernal_sqrtf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fsqr

    .export ___cbmkernal_tanf
___cbmkernal_tanf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __ftan

    .export ___cbmkernal_atanf
___cbmkernal_atanf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fatn

    .export ___cbmkernal_fabsf
___cbmkernal_fabsf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fabs

    .export ___cbmkernal_roundf
___cbmkernal_roundf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fround

    .export ___cbmkernal_truncf
___cbmkernal_truncf:
    ; arg0:     a/x/sreg/sreg+1
    jmp     __fint
