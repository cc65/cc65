;       .org $40      ; SET BASE PAGE ADRESSES
;       SIGN = *

       SIGN = $40

       X2   = SIGN+1        ; EXPONENT 2
       M2   = X2+1          ; MANTISSA 2
       X1   = M2+3          ; EXPONENT 1
       M1   = X1+1          ; MANTISSA 1
       E    = M1+3          ; SCRATCH
       ZZ   = E+4
       T    = ZZ+4
       SEXP = T+4
       INT  = SEXP+4
;

