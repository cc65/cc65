;
; 1998-06-06, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; int __fastcall__ __setjmp (jmp_buf buf);
;

        .export         ___setjmp

        .import         return0
        .importzp       c_sp, ptr1

___setjmp:
        sta     ptr1            ; Save buf
        stx     ptr1+1
        ldy     #0

; The parameter stack is now empty, put it into buf

        lda     c_sp
        sta     (ptr1),y
        iny
        lda     c_sp+1
        sta     (ptr1),y
        iny

; Put the return stack pointer next

        tsx
        inx
        inx                     ; drop return address
        txa
        sta     (ptr1),y
        iny

; Last thing is the return address.

        pla
        tax
        pla
        sta     (ptr1),y        ; high byte first
        iny
        pha
        txa
        sta     (ptr1),y
        pha

; Return zero

        jmp     return0
