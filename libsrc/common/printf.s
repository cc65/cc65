;
; int printf (const char* Format, ...);
;
; Ullrich von Bassewitz, 1.12.2000
;

        .export         _printf
        .import         _stdout, pushax, addysp, _vfprintf
        .importzp       c_sp, ptr1

        .macpack        generic

; ----------------------------------------------------------------------------
; Data

.bss

ParamSize:      .res    1               ; Number of parameter bytes

; ----------------------------------------------------------------------------
; Code

.code


_printf:
        sty     ParamSize               ; Number of param bytes passed in Y

; We are using a (hopefully) clever trick here to reduce code size. On entry,
; the stack pointer points to the last pushed parameter of the variable
; parameter list. Adding the number of parameter bytes, would result in a
; pointer that points *after* the Format parameter.
; Since we have to push stdout anyway, we will do that here, so
;
;   * we will save the subtraction of 2 (__fixargs__) later
;   * we will have the address of the Format parameter which needs to
;     be pushed next.
;

        lda     _stdout
        ldx     _stdout+1
        jsr     pushax

; Now calculate the va_list pointer, which does points to Format

        lda     c_sp
        ldx     c_sp+1
        add     ParamSize
        bcc     @L1
        inx
@L1:    sta     ptr1
        stx     ptr1+1

; Push Format

        ldy     #1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jsr     pushax

; Load va_list (last and __fastcall__ parameter to vfprintf)

        lda     ptr1
        ldx     ptr1+1

; Call vfprintf

        jsr     _vfprintf

; Cleanup the stack. We will return what we got from vfprintf

        ldy     ParamSize
        jmp     addysp

