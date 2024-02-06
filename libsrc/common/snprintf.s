;
; int snprintf (char* buf, size_t size, const char* Format, ...);
;
; Ullrich von Bassewitz, 2009-09-26
;

        .export         _snprintf
        .import         pushax, addysp, decsp6, _vsnprintf
        .importzp       sp, ptr1

        .macpack        generic

; ----------------------------------------------------------------------------
; Data

.bss

ParamSize:      .res    1               ; Number of parameter bytes

; ----------------------------------------------------------------------------
; Code

.code


_snprintf:
        sty     ParamSize               ; Number of param bytes passed in Y

; We have to push buf/size/format, both in the order they already have on stack.
; To make this somewhat more efficient, we will create space on the stack and
; then do a copy of the complete block instead of pushing each parameter
; separately. Since the size of the arguments passed is the same as the size
; of the fixed arguments, this will allow us to calculate the pointer to the
; fixed size arguments easier (they're just ParamSize bytes away).

        jsr     decsp6

; Calculate a pointer to the Format argument

        lda     ParamSize
        add     sp
        sta     ptr1
        ldx     sp+1
        bcc     @L1
        inx
@L1:    stx     ptr1+1

; Now copy buf/size/format

        ldy     #6-1
@L2:    lda     (ptr1),y
        sta     (sp),y
        dey
        bpl     @L2

; Load va_list (last and __fastcall__ parameter to vsprintf)

        lda     ptr1
        ldx     ptr1+1

; Call vsnprintf

        jsr     _vsnprintf

; Cleanup the stack. We will return what we got from vsprintf

        ldy     ParamSize
        jmp     addysp

