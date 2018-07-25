; divt.s
;
; 2002-10-22, Greg King
;
; This signed-division function returns both the quotient and the remainder,
; in this structure:
;
; typedef struct {
;     int rem, quot;
; } div_t;
;
; div_t __fastcall__ div (int numer, int denom);
;

        .export         _div

        .import         tosdivax, negax
        .importzp       sreg, ptr1, tmp1

_div:   jsr     tosdivax        ; Division-operator does most of the work
        lda     sreg            ; Unsigned remainder is in sreg
        ldx     sreg+1
        ldy     ptr1            ; transfer quotient to sreg
        sty     sreg
        ldy     ptr1+1
        sty     sreg+1

; Adjust the sign of the remainder.
; It must be the same as the sign of the dividend.
;
        bit     tmp1            ; Load high-byte of left argument
        bpl     Pos             ; Jump if sign-of-result is positive
        jmp     negax           ; Result is negative, adjust the sign

Pos:    rts

