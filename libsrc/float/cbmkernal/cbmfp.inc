
FAC_EXPONENT    = $61
FAC_MANTISSA0   = $62
FAC_MANTISSA1   = $63
FAC_MANTISSA2   = $64
FAC_MANTISSA3   = $65
FAC_SIGN        = $66

ARG_EXPONENT    = $69
ARG_MANTISSA0   = $6a
ARG_MANTISSA1   = $6b
ARG_MANTISSA2   = $6c
ARG_MANTISSA3   = $6d
ARG_SIGN        = $6e

FAC_SIGN_COMPARE = $6f
FAC_ROUNDING     = $70

; addresses of the floating point routines in CBM BASIC V2

; the following are the addresses in the C64 ROM - the library can probably be
; used with other CBM targets just by adjusting these addresses

BASIC_FAC_Not       = $aed4     ; in/out: FAC
BASIC_FAC_Cos       = $e264     ; in/out: FAC
BASIC_FAC_Sin       = $e26b     ; in/out: FAC
BASIC_FAC_Tan       = $e2b4     ; in/out: FAC
BASIC_FAC_Atn       = $e30e     ; in/out: FAC
BASIC_FAC_Rnd       = $e097     ; in/out: FAC
BASIC_FAC_Int       = $bccc     ; in/out: FAC
BASIC_FAC_Sqr       = $bf71     ; in/out: FAC
BASIC_FAC_Exp       = $bfed     ; in/out: FAC
BASIC_FAC_Log       = $b9ea     ; in/out: FAC
BASIC_FAC_Round     = $bc1b     ; in/out: FAC
BASIC_FAC_Sgn       = $bc39     ; in/out: FAC  "signum" returns 1 if positive, 0 if 0, -1 if negative
BASIC_FAC_Abs       = $bc58     ; in/out: FAC

BASIC_ARG_FAC_Or    = $afe6     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_And   = $afe9     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Sub   = $b853     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Add   = $b86a     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Mul   = $ba2b     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Div   = $bb12     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Pow   = $bf7b     ; in: ARG,FAC out:FAC

BASIC_u8_to_FAC     = $b3a2     ; y: value
BASIC_s8_to_FAC     = $bc3c     ; a: value
BASIC_u16_to_FAC    = $bc49     ; a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...)
BASIC_s16_to_FAC    = $b395     ; a/y:lo/hi value

BASIC_FAC_to_u16    = $bc9b     ; in:FAC out: y/a:lo/hi value

BASIC_string_to_FAC = $b7b5     ; in: $22/$23 ptr to str,a=strlen out: FAC value
BASIC_FAC_to_string = $bddd     ; in: FAC value   out: str at $0100 a/y ptr to str

BASIC_LoadARG       = $ba8c     ; a/y:lo/hi ptr to 5-byte float
BASIC_LoadFAC       = $bba2     ; a/y:lo/hi ptr to 5-byte float

BASIC_FAC_testsgn   = $bc2b     ; in: FAC(x1) out: a=0 (x1=0) a=1 (x1>0) a=255 (x1<0)
BASIC_FAC_cmp       = $bc5b     ; in: FAC(x1) a/y ptr lo/hi to x2 out: a=0 (x1=x2) a=1 (x1>x2) a=255 (x1<x2)

BASIC_FAC_Poly2     = $e043     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient)
BASIC_FAC_Poly1     = $e059     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient)

