;
; int scanf(const char* Format, ...);
;
; 2000-12-01, Ullrich von Bassewitz
; 2004-12-31, Greg King
;

        .export         _scanf

        .import         _stdin, pushax, addysp, _vfscanf
        .import         sp:zp, ptr1:zp

        .macpack        generic

; ----------------------------------------------------------------------------
; Code
;
_scanf:
        sty     ArgSize         ; Number of argument bytes passed in .Y

; We are using a (hopefully) clever trick here to reduce code size.  On entry,
; the stack pointer points to the last pushed argument of the variable
; argument list.  Adding the number of argument bytes, would result in a
; pointer that points _above_ the Format argument.
; Because we have to push stdin anyway, we will do that here, so:
;
;   * we will save the subtraction of 2 (__fixargs__) later;
;   * we will have the address of the Format argument which needs to
;     be pushed next.

        lda     _stdin
        ldx     _stdin+1
        jsr     pushax

; Now, calculate the va_list pointer, which does point to Format.

        lda     sp
        ldx     sp+1
        add     ArgSize
        bcc     @L1
        inx
@L1:    sta     ptr1
        stx     ptr1+1

; Push a copy of Format.

        ldy     #1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jsr     pushax

; Load va_list [last and __fastcall__ argument to vfscanf()].

        lda     ptr1
        ldx     ptr1+1

; Call vfscanf().

        jsr     _vfscanf

; Clean up the stack.  We will return what we got from vfscanf().

        ldy     ArgSize
        jmp     addysp

; ----------------------------------------------------------------------------
; Data
;
        .bss
ArgSize:
        .res    1               ; Number of argument bytes

