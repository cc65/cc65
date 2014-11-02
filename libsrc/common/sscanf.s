;
; int sscanf (const char* str, const char* format, ...);
;
; Ullrich von Bassewitz, 2004-11-28
;

        .export         _sscanf
        .import         addysp, decsp4, _vsscanf
        .importzp       sp, ptr1

        .macpack        generic

; ----------------------------------------------------------------------------
; Data

.bss

ParamSize:      .res    1               ; Number of parameter bytes

; ----------------------------------------------------------------------------
; Code
; int sscanf (const char* str, const char* format, ...)
; /* Standard C function */
; {
;     va_list ap;
;
;     /* Setup for variable arguments */
;     va_start (ap, format);
;
;     /* Call vsscanf(). Since we know that va_end won't do anything, we will
;     ** save the call and return the value directly.
;     */
;     return vsscanf (str, format, ap);
; }
;

.code

_sscanf:
        sty     ParamSize               ; Number of param bytes passed in Y

; We have to push buf and format, both in the order they already have on stack.
; To make this somewhat more efficient, we will create space on the stack and
; then do a copy of the complete block instead of pushing each parameter
; separately. Since the size of the arguments passed is the same as the size
; of the fixed arguments, this will allow us to calculate the pointer to the
; fixed size arguments easier (they're just ParamSize bytes away).

        jsr     decsp4

; Calculate a pointer to the fixed parameters

        lda     ParamSize
        add     sp
        sta     ptr1
        ldx     sp+1
        bcc     @L1
        inx
@L1:    stx     ptr1+1

; Now copy both, str and format

        ldy     #4-1
@L2:    lda     (ptr1),y
        sta     (sp),y
        dey
        bpl     @L2

; Load va_list (last and __fastcall__ parameter to vsscanf)

        lda     ptr1
        ldx     ptr1+1

; Call vsscanf

        jsr     _vsscanf

; Cleanup the stack. We will return what we got from vsscanf

        ldy     ParamSize
        jmp     addysp

