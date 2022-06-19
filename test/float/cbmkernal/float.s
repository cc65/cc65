
BINARYFORMAT_CBM_UNPACKED   = 0
BINARYFORMAT_CBM_PACKED     = 1
BINARYFORMAT_IEEE754        = 2

; BEWARE: also change in float.h
;BINARYFORMAT = BINARYFORMAT_CBM_UNPACKED
;BINARYFORMAT = BINARYFORMAT_CBM_PACKED
BINARYFORMAT = BINARYFORMAT_IEEE754

        .segment "LOWCODE"

;---------------------------------------------------------------------------------------------

.if .defined(__C64__)
__basicon:
        sei
        ldx #$37
        stx $01
        rts

__basicoff:
        ldx #$36
        stx $01
        cli
        rts
.endif

.macro __enable_basic_if_needed
  .if .defined(__C64__)
        jsr __basicon
  .endif
.endmacro

.macro __disable_basic_if_needed
  .if .defined(__C64__)
        jsr __basicoff
  .endif
.endmacro

.macro __return_with_cleanup
  .if .defined(__C64__)
        jmp __basicoff
  .else
        rts
  .endif
.endmacro

;---------------------------------------------------------------------------------------------
; first come the actual stubs to floating point routines, these are ment to be
; used from further written ml-math routines aswell. (maybe also from the compiler?!)
;---------------------------------------------------------------------------------------------

.if .defined(__VIC20__)
        .include "float-vic20.inc"
.else
        .include "float.inc"
.endif
        
        .importzp sreg, ptr1

;---------------------------------------------------------------------------------------------
; converter integer types to float
;---------------------------------------------------------------------------------------------

___float_s8_to_fac:
        ;a: low
__float_s8_to_fac:
        __enable_basic_if_needed
        jsr BASIC_s8_to_FAC
        __return_with_cleanup

___float_u8_to_fac:
        ;a: low
        tay
        ;y: low
__float_u8_to_fac:
        __enable_basic_if_needed
        jsr BASIC_u8_to_FAC
        __return_with_cleanup
        
; get C-parameter (signed int), convert to FAC
___float_s16_to_fac:
        ;a: low x: high
        tay
        txa
        ;y: low a: high
        
; convert signed int (YA) to FAC
__float_s16_to_fac:
        __enable_basic_if_needed           ; enable BASIC (trashes X)
        jsr BASIC_s16_to_FAC
        __return_with_cleanup
       
; get C-parameter (unsigned short), convert to FAC          
___float_u16_to_fac:
        ;a: low x: high
        tay
        txa
        ;y: low a: high
        
__float_u16_to_fac:
        sta FAC_MANTISSA0
        sty FAC_MANTISSA1
        __enable_basic_if_needed
        ldx #$90
        sec
        jsr BASIC_u16_to_FAC
        __return_with_cleanup

; return to C, FAC as unsigned int
__float_fac_to_u16:
        __enable_basic_if_needed
        jsr BASIC_FAC_to_u16
        __disable_basic_if_needed
        ldx FAC_MANTISSA2
        lda FAC_MANTISSA3
        rts
        
;---------------------------------------------------------------------------------------------
; converter float to string and back
;---------------------------------------------------------------------------------------------

; this converts to exponential form, ie what %e in printf would give you
__float_fac_to_str:
        __enable_basic_if_needed
        jsr BASIC_FAC_to_string
        __return_with_cleanup

___float_str_to_fac:
;        jsr popax
__float_str_to_fac:
        sta $22
        stx $23
        ldy #$00
@l:     lda ($22),y
        beq @s
        iny
        bne @l
@s:     tya
        __enable_basic_if_needed
        jsr BASIC_string_to_FAC
        __return_with_cleanup

;---------------------------------------------------------------------------------------------

; get C-parameter (float), convert to FAC
___float_float_to_fac:
.if BINARYFORMAT = BINARYFORMAT_CBM_UNPACKED
        sta FAC_MANTISSA1   ; 3
        stx FAC_MANTISSA0   ; 2
        ldy sreg            ; 1
        sty FAC_EXPONENT    
        ldy sreg+1          ; 0
        sty FAC_SIGN        

        ldx #$00
        stx FAC_MANTISSA2
        stx FAC_MANTISSA3
        stx FAC_ROUNDING
.endif
.if BINARYFORMAT = BINARYFORMAT_CBM_PACKED
        sta FAC_MANTISSA2   ; 3
        stx FAC_MANTISSA1   ; 2
        lda sreg            ; 1
        ora #$80
        sta FAC_MANTISSA0   

        ; bit7=0 sign=0
        ; bit7=1 sign=$ff
        ldx #0
        lda sreg
        bpl @pos
        dex
@pos:
        stx FAC_SIGN

        ldy sreg+1          ; 0
        sty FAC_EXPONENT    

        ldx #$00
        stx FAC_MANTISSA3
        stx FAC_ROUNDING
.endif
.if BINARYFORMAT = BINARYFORMAT_IEEE754
        ; ieee float is in A/X/sreg/sreg+1
        sta FAC_MANTISSA2
        stx FAC_MANTISSA1

        ; shift msb from mantissa into exponent
        asl sreg        ; mantissa msb
        rol sreg+1      ; exp

        ; sign is in carry
        lda #$ff
        bcs @l
        lda #0
@l:
        sta FAC_SIGN

        lda sreg        ; mantissa msb
        lsr
        ora #$80        ; msb of mantissa is always 1
        sta FAC_MANTISSA0

        lda sreg+1      ; exp
        clc
        adc #2
        sta FAC_EXPONENT

        ldx #$00
        stx FAC_MANTISSA3
        stx FAC_ROUNDING
.endif
        rts
        
; load BASIC float into FAC        
; in: pointer (a/x) to BASIC float (not packed)        
__float_float_to_fac:   ; only used in ATAN2?
        sta ptr1
        stx ptr1+1
        ldy #$00
        lda (ptr1),y
        sta FAC_EXPONENT
        iny
        lda (ptr1),y
        sta FAC_MANTISSA0
        iny
        lda (ptr1),y
        sta FAC_MANTISSA1
        iny
        lda (ptr1),y
        sta FAC_MANTISSA2
        iny
        lda (ptr1),y
        sta FAC_MANTISSA3
        iny
        lda (ptr1),y
        sta FAC_SIGN
        ldx #$00
        stx FAC_ROUNDING
        ; always load arg after fac so these can
        ; be removed in funcs that only take fac
        eor ARG_SIGN
        sta FAC_SIGN_COMPARE
        rts

; get C-parameter (two floats), to FAC and ARG
___float_float_to_fac_arg:
        jsr ___float_float_to_fac
___float_float_to_arg:
        ldy #$03
        jsr ldeaxysp
        
.if BINARYFORMAT = BINARYFORMAT_CBM_UNPACKED
        sta ARG_MANTISSA1   ; 3
        stx ARG_MANTISSA0   ; 2
        ldy sreg            ; 1
        sty ARG_EXPONENT

        ldx #$00
        stx ARG_MANTISSA2
        stx ARG_MANTISSA3

        lda sreg+1          ; 0
        sta ARG_SIGN
        eor FAC_SIGN
        sta FAC_SIGN_COMPARE ; sign compare
.endif

.if BINARYFORMAT = BINARYFORMAT_CBM_PACKED
        sta ARG_MANTISSA2   ; 3
        stx ARG_MANTISSA1   ; 2
        lda sreg            ; 1
        ora #$80
        sta ARG_MANTISSA0   
        
        ; bit7=0 sign=0
        ; bit7=1 sign=$ff
        ldx #0
        lda sreg            ; 1
        bpl @pos
        dex
@pos:
        stx ARG_SIGN
        
        ldy sreg+1          ; 0
        sty ARG_EXPONENT    

        ldx #$00
        stx ARG_MANTISSA3
        
        lda ARG_SIGN
        eor FAC_SIGN
        sta FAC_SIGN_COMPARE ; sign compare
.endif


.if BINARYFORMAT = BINARYFORMAT_IEEE754
        ; ieee float in a/x/sreg/sreg+1
        sta ARG_MANTISSA2
        stx ARG_MANTISSA1

        asl sreg        ; mantissa msb
        rol sreg+1      ; exp

        ; sign is in carry
        lda #$ff
        bcs @l
        lda #0
@l:
        sta ARG_SIGN

        lda sreg        ; mantissa msb
        lsr
        ora #$80        ; the first bit in the mantissa should always be 1
        sta ARG_MANTISSA0

        lda sreg+1      ; exp
        clc
        adc #2
        sta ARG_EXPONENT

        ldx #$00
        stx ARG_MANTISSA3

        lda ARG_SIGN
        eor FAC_SIGN
        sta FAC_SIGN_COMPARE ; sign compare
.endif

        jmp incsp4

; load BASIC float into ARG
; in: pointer (a/x) to BASIC float (not packed)        
__float_float_to_arg:   ; only used in ATAN2?
        sta ptr1
        stx ptr1+1
        ldy #$00
        lda (ptr1),y
        sta ARG_EXPONENT
        iny
        lda (ptr1),y
        sta ARG_MANTISSA0
        iny
        lda (ptr1),y
        sta ARG_MANTISSA1
        iny
        lda (ptr1),y
        sta ARG_MANTISSA2
        iny
        lda (ptr1),y
        sta ARG_MANTISSA3
        iny
        lda (ptr1),y
        sta ARG_SIGN
        ; sign compare
        eor FAC_SIGN
        sta FAC_SIGN_COMPARE
        rts
        
; return to C, float as unsigned long
___float_fac_to_float:
.if BINARYFORMAT = BINARYFORMAT_CBM_UNPACKED
        lda FAC_SIGN
        sta sreg+1          ; 0
        lda FAC_EXPONENT
        sta sreg            ; 1
        ldx FAC_MANTISSA0   ; 2
        lda FAC_MANTISSA1   ; 3
.endif        
.if BINARYFORMAT = BINARYFORMAT_CBM_PACKED
        lda FAC_EXPONENT
        sta sreg+1          ; 0
        
        ; use the MSB of the mantissa for the sign
        lda FAC_SIGN        ; either $ff or $00
        ora #$7f            ; ->     $ff or $7f
        and FAC_MANTISSA0   ; bit7 of mantissa is always 1
        sta sreg            ; 1

        ldx FAC_MANTISSA1   ; 2
        lda FAC_MANTISSA2   ; 3
.endif        
.if BINARYFORMAT = BINARYFORMAT_IEEE754
        ; return float in a/x/sreg/sreg+1
        lda FAC_EXPONENT
        sec
        sbc #2
        sta sreg+1          ; 0

        lda FAC_MANTISSA0
        asl
        sta sreg            ; 1

        lda FAC_SIGN        ; either $ff or $00
        asl
        ror sreg+1      ; exp
        ror sreg        ; mantissa msb


        ldx FAC_MANTISSA1   ; 2
        lda FAC_MANTISSA2   ; 3 lsb

.endif        
        rts        

;; store float in memory        
;; in: dest. pointer (a/x), float in FAC
;__float_fac_to_float:   ; UNUSED
;        sta ptr1
;        stx ptr1+1
;        ldy #$00
;        lda FAC_EXPONENT
;        sta (ptr1),y
;        iny
;        lda FAC_MANTISSA0
;        sta (ptr1),y
;        iny
;        lda FAC_MANTISSA1
;        sta (ptr1),y
;        iny
;        lda FAC_MANTISSA2
;        sta (ptr1),y
;        iny
;        lda FAC_MANTISSA3
;        sta (ptr1),y
;        iny
;        lda FAC_SIGN
;        sta (ptr1),y
;        rts

;; store packed float in memory        
;; in: dest. pointer (a/x), float in FAC        
;__float_fac_to_float_packed:    ; UNUSED
;        sta ptr1
;        stx ptr1+1
;        ldy #4
;        lda FAC_MANTISSA3
;        sta (ptr1),y
;        dey
;        lda FAC_MANTISSA2
;        sta (ptr1),y
;        dey
;        lda FAC_MANTISSA1
;        sta (ptr1),y
;        dey
;; use the MSB of the mantissa for the sign
;        lda FAC_SIGN
;        ora #$7f
;        and FAC_MANTISSA0
;        sta (ptr1),y
;        dey
;        lda FAC_EXPONENT
;        sta (ptr1),y
;        rts
        
;; store packed float in memory        
;; in: dest. pointer (a/x), float in ARG
__float_arg_to_float_packed:
        sta ptr1
        stx ptr1+1
        ldy #4
        lda ARG_MANTISSA3
        sta (ptr1),y
        dey
        lda ARG_MANTISSA2
        sta (ptr1),y
        dey
        lda ARG_MANTISSA1
        sta (ptr1),y
        dey
; use the MSB of the mantissa for the sign
        lda ARG_SIGN
        ora #$7f
        and ARG_MANTISSA0
        sta (ptr1),y
        dey
        lda ARG_EXPONENT
        sta (ptr1),y
        rts
        
;---------------------------------------------------------------------------------------------

        .export __ftostr
        .importzp ptr1
        .import popax, ldeaxysp, incsp4

; convert float to string
; char* __fastcall__ _ftostr(char *d, float s);
;-> char* __fastcall__ _ftostr(char *d, unsigned long s);

__ftostr:
        jsr ___float_float_to_fac
        jsr __float_fac_to_str

___float_strbuf_to_string:
        jsr popax ; ptr to string
__float_strbuf_to_string:
        sta ptr1
        stx ptr1+1
        ldy #$00
@l:
        lda $0100,y
        sta (ptr1),y
        beq @s
        iny
        bne @l
@s:
        lda ptr1
        ldx ptr1+1
        rts

        .export __strtof
        
; convert a string to a float        
; float __fastcall__ _strtof(char *d);        
;-> unsigned long __fastcall__ _strtof(char *d);        
__strtof:
        jsr ___float_str_to_fac
        jmp ___float_fac_to_float

        .export __ctof

; convert char to float
; float __fastcall__ _ctof(char v);
;-> unsigned long __fastcall__ _ctof(char v);
 __ctof:
        jsr ___float_s8_to_fac
        jmp ___float_fac_to_float

        .export __utof
        
; convert unsigned char to float
; float __fastcall__ _utof(unsigned char v);
;-> unsigned long __fastcall__ _utof(unsigned char v);
 __utof:
        jsr ___float_u8_to_fac
        jmp ___float_fac_to_float

        .export __stof
        
; convert short to float
; float __fastcall__ _stof(unsigned short v);
;-> unsigned long __fastcall__ _stof(unsigned short v);
 __stof:
        jsr ___float_u16_to_fac
        jmp ___float_fac_to_float

        .export __itof

; convert integer to float
; float __fastcall__ _itof(int v);
;-> unsigned long __fastcall__ _itof(int v);
 __itof:
        ;a: low x: high
        jsr ___float_s16_to_fac
        jmp ___float_fac_to_float

        .export __ftoi
        
; convert float to integer
; int __fastcall__ _ftoi(float f);
;-> int __fastcall__ _ftoi(unsigned long f);
 __ftoi:
        jsr ___float_float_to_fac
        jmp __float_fac_to_u16

;---------------------------------------------------------------------------------------------
; these functions take one arg (in FAC) and return result (in FAC) aswell
;---------------------------------------------------------------------------------------------

.macro __ffunc1 addr
        jsr ___float_float_to_fac
        __enable_basic_if_needed
        jsr addr
        __disable_basic_if_needed
        jmp ___float_fac_to_float
.endmacro

        .export __fabs, __fatn, __fcos, __fexp, __fint, __flog
        .export __frnd, __fsgn, __fsin, __fsqr, __ftan, __fnot, __fround

__fabs:    __ffunc1 BASIC_FAC_Abs
__fatn:    __ffunc1 BASIC_FAC_Atn
__fcos:    __ffunc1 BASIC_FAC_Cos
__fexp:    __ffunc1 BASIC_FAC_Exp
;__ffre:    __ffunc1 BASIC_FAC_Fre
__fint:    __ffunc1 BASIC_FAC_Int
__flog:    __ffunc1 BASIC_FAC_Log
;__fpos:    __ffunc1 BASIC_FAC_Pos
__frnd:    __ffunc1 BASIC_FAC_Rnd
__fsgn:    __ffunc1 BASIC_FAC_Sgn
__fsin:    __ffunc1 BASIC_FAC_Sin
__fsqr:    __ffunc1 BASIC_FAC_Sqr
__ftan:    __ffunc1 BASIC_FAC_Tan
__fnot:    __ffunc1 BASIC_FAC_Not
__fround:  __ffunc1 BASIC_FAC_Round
        
;---------------------------------------------------------------------------------------------
; these functions take two args (in FAC and ARG) and return result (in FAC)
;---------------------------------------------------------------------------------------------
        
__float_ret2:

        ;jsr __basicoff
.if .defined(__C64__)
        ldx #$36
        stx $01
        cli
.endif
        jmp ___float_fac_to_float    ; also pops pointer to float

.macro __ffunc2a addr
        jsr ___float_float_to_fac_arg
        __enable_basic_if_needed
        lda FAC_EXPONENT
        jsr addr
        jmp __float_ret2
.endmacro

.macro __ffunc2b addr
        jsr ___float_float_to_fac_arg
        __enable_basic_if_needed
        jsr addr
        jmp __float_ret2
.endmacro
        
        .export __fadd, __fsub, __fmul, __fdiv, __fpow

; float __fastcall__ _fadd(float f, float a);        
;-> unsigned long __fastcall__ _fadd(unsigned long f, unsigned long a);        
__fadd:   __ffunc2a BASIC_ARG_FAC_Add
__fsub:   __ffunc2a BASIC_ARG_FAC_Sub
__fmul:   __ffunc2a BASIC_ARG_FAC_Mul
__fdiv:   __ffunc2a BASIC_ARG_FAC_Div
__fpow:   __ffunc2a BASIC_ARG_FAC_Pow

        .export __fand, __for

__fand:   __ffunc2b BASIC_ARG_FAC_And
__for:    __ffunc2b BASIC_ARG_FAC_Or
        
__float_ret3:
        ;jsr __basicoff
.if .defined(__C64__)
        ldx #$36
        stx $01
        cli
.endif
        ldx #0
        rts  
        
        .bss
        
tempfloat:
        .res 5

        .SEGMENT "LOWCODE"
        
        .export __fcmp
        
__fcmp:
        jsr ___float_float_to_fac_arg
        lda #<tempfloat
        ldx #>tempfloat
        jsr __float_arg_to_float_packed
        lda #<tempfloat
        ldy #>tempfloat
___float_cmp_fac_arg:
        __enable_basic_if_needed
        ; in: FAC=(x1) a/y= ptr lo/hi to x2
        jsr BASIC_FAC_cmp
        ; a=0 (==) / a=1 (>) / a=255 (<)
        jmp __float_ret3

        .export __ftestsgn
        
__ftestsgn:
        jsr ___float_float_to_fac
;___float_testsgn_fac:
        __enable_basic_if_needed
        ; in: FAC(x1)
        jsr BASIC_FAC_testsgn
        jmp __float_ret3
        
___float_testsgn_fac:
        lda FAC_EXPONENT
        beq @s
        lda FAC_SIGN
        rol a
        lda #$ff
        bcs @s
        lda #$01
@s:
        rts

___float_testsgn_arg:
        lda ARG_EXPONENT
        beq @s
        lda ARG_SIGN
        rol a
        lda #$ff
        bcs @s
        lda #$01
@s:
        rts

;---------------------------------------------------------------------------------------------
; polynom1 f(x)=a1+a2*x^2+a3*x^3+...+an*x^n
;---------------------------------------------------------------------------------------------
        .export __fpoly1
__fpoly1:
        jsr ___float_float_to_fac
        ;jsr popya
        jsr popax
        tay
        txa
        __enable_basic_if_needed
        jsr BASIC_FAC_Poly1
        jmp __float_ret2

;---------------------------------------------------------------------------------------------
; polynom2 f(x)=a1+a2*x^3+a3*x^5+...+an*x^(2n-1)
;---------------------------------------------------------------------------------------------
        .export __fpoly2
__fpoly2:
        jsr ___float_float_to_fac
        ;jsr popya
        jsr popax
        tay
        txa
        __enable_basic_if_needed
        jsr BASIC_FAC_Poly1
        jmp __float_ret2
        
;---------------------------------------------------------------------------------------------
        
__float_atn_fac:
        __enable_basic_if_needed
        jsr BASIC_FAC_Atn
        __return_with_cleanup
__float_div_fac_arg:
        __enable_basic_if_needed
        lda FAC_EXPONENT
        jsr BASIC_ARG_FAC_Div
        __return_with_cleanup
__float_add_fac_arg:
        __enable_basic_if_needed
        lda FAC_EXPONENT
        jsr BASIC_ARG_FAC_Add
        __return_with_cleanup
        
__float_swap_fac_arg:           ; only used in ATAN2
        lda   FAC_EXPONENT
        ldx   ARG_EXPONENT
        stx   FAC_EXPONENT
        sta   ARG_EXPONENT
        lda   FAC_MANTISSA0
        ldx   ARG_MANTISSA0
        stx   FAC_MANTISSA0
        sta   ARG_MANTISSA0
        lda   FAC_MANTISSA1
        ldx   ARG_MANTISSA1
        stx   FAC_MANTISSA1
        sta   ARG_MANTISSA1
        lda   FAC_MANTISSA2
        ldx   ARG_MANTISSA2
        stx   FAC_MANTISSA2
        sta   ARG_MANTISSA2
        lda   FAC_MANTISSA3
        ldx   ARG_MANTISSA3
        stx   FAC_MANTISSA3
        sta   ARG_MANTISSA3
        lda   FAC_SIGN
        ldx   ARG_SIGN
        stx   FAC_SIGN
        sta   ARG_SIGN
        rts
        
        .export __fneg
__fneg:
        jsr ___float_float_to_fac

        lda FAC_EXPONENT
        beq @sk
        lda FAC_SIGN
        eor #$FF
        sta FAC_SIGN
@sk:
        jmp ___float_fac_to_float
        
        
__f_pi2:  .byte $81,$80+$49,$0f,$da,$a1,$00
__f_pi:   .byte $82,$80+$49,$0f,$da,$a1,$00
__f_1pi2: .byte $83,$80+$16,$cb,$e3,$f9,$00

        .export __fatan2

; float _fatan2(float x, float y)
;-> unsigned long _fatan2(unsigned long x, unsigned long y)
__fatan2:

        jsr ___float_float_to_fac_arg

        jsr ___float_testsgn_arg
        beq @s11   ; =0
        bpl @s12   ; <0
      ; arg>0
                ; a=atn(y/x)
                jsr __float_swap_fac_arg
                jsr __float_div_fac_arg
                jsr __float_atn_fac
                jmp __float_ret2
@s12: ; arg<0
                ; a=atn(y/x)+pi
                jsr __float_swap_fac_arg
                jsr __float_div_fac_arg
                jsr __float_atn_fac
                lda #<__f_pi
                ldx #>__f_pi
                jsr __float_float_to_arg
                jsr __float_add_fac_arg
                jmp __float_ret2

@s11: ; arg=0
                jsr ___float_testsgn_fac
                beq @s21   ; =0
                bpl @s22   ; <0
      ; fac >0
                        ; a= 0.5*pi
                        lda #<__f_pi2
                        ldx #>__f_pi2
                        jsr __float_float_to_fac
                        jmp __float_ret2
      ; fac =0
@s21:
                        ; a= 0
                        lda #$00
                        sta FAC_MANTISSA0
                        jmp __float_ret2
      ; fac <0
@s22:
                        ; a= 1.5*pi
                        lda #<__f_1pi2
                        ldx #>__f_1pi2
                        jsr __float_float_to_fac
                        jmp __float_ret2
        
