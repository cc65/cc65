
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

; the following are the addresses in the VIC-20 ROM - the library can probably be
; used with other CBM targets just by adjusting these addresses

BASIC_FAC_Not       = $ced4     ; in/out: FAC ; yup, c64 & vic are this different
BASIC_FAC_Cos       = $e261     ; in/out: FAC
BASIC_FAC_Sin       = $e268     ; in/out: FAC
BASIC_FAC_Tan       = $e2b1     ; in/out: FAC
BASIC_FAC_Atn       = $e30b     ; in/out: FAC
BASIC_FAC_Rnd       = $e094     ; in/out: FAC
BASIC_FAC_Int       = $dccc     ; in/out: FAC
BASIC_FAC_Sqr       = $df71     ; in/out: FAC
BASIC_FAC_Exp       = $dfed     ; in/out: FAC
BASIC_FAC_Log       = $d9ea     ; in/out: FAC
BASIC_FAC_Round     = $dc1b     ; in/out: FAC
BASIC_FAC_Sgn       = $dc39     ; in/out: FAC
BASIC_FAC_Abs       = $dc58     ; in/out: FAC

BASIC_ARG_FAC_Or    = $cfe6     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_And   = $cfe9     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Sub   = $d853     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Add   = $d86a     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Mul   = $da2b     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Div   = $db12     ; in: ARG,FAC out:FAC
BASIC_ARG_FAC_Pow   = $df7b     ; in: ARG,FAC out:FAC

BASIC_u8_to_FAC     = $d3a2     ; y: value
BASIC_s8_to_FAC     = $dc3c     ; a: value
BASIC_u16_to_FAC    = $dc49     ; a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...)
BASIC_s16_to_FAC    = $d395     ; a/y:lo/hi value

BASIC_FAC_to_u16    = $dc9b     ; in:FAC out: y/a:lo/hi value

BASIC_string_to_FAC = $d7b5     ; in: $22/$23 ptr to str,a=strlen out: FAC value
BASIC_FAC_to_string = $dddd     ; in: FAC value   out: str at $0100 a/y ptr to str

BASIC_LoadARG       = $da8c     ; a/y:lo/hi ptr to 5-byte float
BASIC_LoadFAC       = $dba2     ; a/y:lo/hi ptr to 5-byte float

BASIC_FAC_testsgn   = $dc2b     ; in: FAC(x1) out: a=0 (x1=0) a=1 (x1>0) a=255 (x1<0)
BASIC_FAC_cmp       = $dc5b     ; in: FAC(x1) a/y ptr lo/hi to x2 out: a=0 (x1=x2) a=1 (x1>x2) a=255 (x1<x2)

BASIC_FAC_Poly2     = $e040     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient)
BASIC_FAC_Poly1     = $e056     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient)
