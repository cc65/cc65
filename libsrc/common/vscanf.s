;
; int __fastcall__ vscanf (const char* format, va_list ap);
;
; Ullrich von Bassewitz, 2004-11-27
;

        .export         _vscanf
        .import         _vfscanf
        .import         _stdin
        .import         decsp2

        .include        "zeropage.inc"


; ----------------------------------------------------------------------------
; int __fastcall__ vscanf (const char* format, va_list ap)
; /* Standard C function */
; {
;     return vfscanf (stdin, format, ap);
; }
;

.code
_vscanf:
        pha                     ; Save low byte of ap

; Decrement the stack pointer by two for the additional parameter.

        jsr     decsp2          ; Won't touch X

; Move the format down

        ldy     #2
        lda     (c_sp),y        ; Load byte of format
        ldy     #0
        sta     (c_sp),y
        ldy     #3
        lda     (c_sp),y
        ldy     #1
        sta     (c_sp),y

; Store stdin into the stack frame

        iny
        lda     _stdin
        sta     (c_sp),y
        iny
        lda     _stdin+1
        sta     (c_sp),y

; Restore the low byte of ap and jump to vfscanf, which will cleanup the stack

        pla
        jmp     _vfscanf

