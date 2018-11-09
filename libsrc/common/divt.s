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
        
        ldy     sreg            ; low byte remainder from sreg
        sta     sreg            ; store low byte quotient to sreg
        
        lda     sreg+1          ; high byte remainder from sreg
        stx     sreg+1          ; store high byte quotient to sreg

        tax                     ; high byte remainder to x
        tya                     ; low byte remainder to a

; Adjust the sign of the remainder.
; It must be the same as the sign of the dividend.
;
        bit     tmp1            ; Load high-byte of left argument
        bpl     Pos             ; Jump if sign-of-result is positive
        jmp     negax           ; Result is negative, adjust the sign

Pos:    rts
