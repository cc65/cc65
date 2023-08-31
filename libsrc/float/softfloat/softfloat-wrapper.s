
    .importzp sreg, tmp1

    .import _int32_to_float32

;------------------------------------------------------------------------------
; conversions: to float

    ; 16bit signed -> float
    .export axfloat
axfloat:
    ; FIXME
    ; sign extend to 32bit
    ldy #$ff
    cpx #$80
    bcs @sk
    ldy #$00
@sk:
    sty sreg
    sty sreg+1
    jmp _int32_to_float32

    ; 16bit unsigned -> float
    .export axufloat
axufloat:
    ; FIXME
    ldy #0
    sty sreg
    sty sreg+1
    jmp _int32_to_float32

    ; 32bit signed -> float
    .export eaxfloat
eaxfloat:
    ; FIXME
    jmp _int32_to_float32

    ; 32bit unsigned -> float
    .export eaxufloat
eaxufloat:
    ; FIXME
    jmp _int32_to_float32

;------------------------------------------------------------------------------
; conversions: from float

    .import _float32_to_int32
    .import _float32_to_int32_round_to_zero

    ; float -> 16bit int
    .export feaxint
feaxint:
    ; FIXME
    jmp _float32_to_int32_round_to_zero
;    jmp _float32_to_int32
    ; float -> 32bit int
    .export feaxlong
feaxlong:
    jmp _float32_to_int32_round_to_zero
;    jmp _float32_to_int32

;------------------------------------------------------------------------------
; the ! operator, returns a bool (int)
    .export fbnegeax
fbnegeax:
    stx     tmp1
    ora     tmp1
    ora     sreg
    ora     sreg+1
    beq     @ret1   ; is = 0
    ; return 0
    lda #0
    tax
    rts
@ret1:
    lda #1
    ldx #0
    rts

    .import _float32_mul
    ; arg0:     a/x/sreg/sreg+1
    .export fnegeax
fnegeax:
    pha
;    txa
;    pha
    lda     sreg+1
    eor     #$80 ; sign bit
    sta     sreg+1
;    lda     sreg+0
;    eor     #$ff ; sign bit
;    sta     sreg+0
;    txa
;    eor     #$ff ; sign bit
;    tax
;    pla
;    tax
    pla
;    eor     #$ff ; sign bit
    rts


    .import _float32_add
    .import _float32_sub
    .import _float32_mul
    .import _float32_div

    .export ftosaddeax
ftosaddeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
     jmp _float32_add
    .export ftossubeax
ftossubeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
     jmp _float32_sub
    .export ftosmuleax
ftosmuleax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
     jmp _float32_mul
    .export ftosdiveax
ftosdiveax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
     jmp _float32_div

    .import _float32_rsub
    .export ftosrsubeax
ftosrsubeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
     jmp _float32_rsub

     .import _float32_eq
     .import _float32_le
     .import _float32_lt

    ; test for equal
    .export ftoseqeax
ftoseqeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jmp _float32_eq

    ; test for not equal
    .export ftosneeax
ftosneeax:
    ; arg0:     a/x/sreg/sreg+1
    ; arg1:     (sp),y (y=0..3)
    jsr _float32_eq
    eor #1
    rts


    ; Test for less than or equal to
    .export ftosleeax
ftosleeax:
    jmp _float32_le

    .export ftosgteax
ftosgteax:
    jsr _float32_le
    eor #1
    rts

    ; Test for less than
    .export ftoslteax
ftoslteax:
    jmp _float32_lt

    ; Test for "not less than" -> "equal or greater than"
    ; Test for greater than or equal to
    .export ftosgeeax
ftosgeeax:
    jsr _float32_lt
    eor #1
    rts
