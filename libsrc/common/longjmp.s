;
; 1998-06-06, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void __fastcall__ longjmp (jmp_buf buf, int retval);
;

        .export         _longjmp
        .import         popax
        .importzp       sp, ptr1, ptr2

_longjmp:
        sta     ptr2            ; Save retval
        stx     ptr2+1
        ora     ptr2+1          ; Check for 0
        bne     @L1
        inc     ptr2            ; 0 is illegal, according to the standard ...
                                ; ... and, must be replaced by 1
@L1:    jsr     popax           ; get buf
        sta     ptr1
        stx     ptr1+1
        ldy     #0

; Get the old parameter stack

        lda     (ptr1),y
        iny
        sta     sp
        lda     (ptr1),y
        iny
        sta     sp+1

; Get the old stack pointer

        lda     (ptr1),y
        iny
        tax
        txs

; Get the return address and push it on the stack

        lda     (ptr1),y
        iny
        pha
        lda     (ptr1),y
        pha

; Load the return value and return to the caller

        lda     ptr2
        ldx     ptr2+1
        rts
