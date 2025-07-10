; vfprintf.s
;
; int fastcall vfprintf(FILE* f, const char* Format, va_list ap);
;
; 2005-02-08, Ullrich von Bassewitz
; 2005-02-11, Greg King

        .export         _vfprintf
        .import         push1, pushwysp, incsp6
        .import         _fwrite, __printf
        .importzp       c_sp, ptr1

        .macpack        generic


.data

; ----------------------------------------------------------------------------
; Static data for the _vfprintf routine
;
outdesc:                        ; Static outdesc structure
ccount: .res    2
        .word   out             ; Output function pointer
ptr:    .res    2               ; Points to output file
        .res    2               ; (Not used by this function)

.code

; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; Since we know, that this routine is always called with "our" outdesc, we
; can ignore the passed pointer d, and access the data directly. While this
; is not very clean, it gives better and shorter code.
;
; static void cdecl out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
; {
;     register size_t cnt;
;
;     /* Write to the file */
;     if ((cnt = fwrite(buf, 1, count, ptr)) == 0) {
;         ccount = -1;
;     } else {
;         ccount += cnt;
;     }
; }

; About to call
;
;       fwrite (buf, 1, count, ptr);
;
out:    ldy     #5
        jsr     pushwysp        ; Push buf
        jsr     push1           ; Push #1
        ldy     #7
        jsr     pushwysp        ; Push count
        lda     ptr
        ldx     ptr+1
        jsr     _fwrite
        sta     ptr1            ; Save function result
        stx     ptr1+1

; Check the return value.

        ora     ptr1+1
        bne     @Ok

; We had an error. Store -1 into ccount

.ifp02
        lda     #<-1
.else
        dec     a
.endif
        sta     ccount
        bne     @Done           ; Branch always

; Result was ok, count bytes written

@Ok:    lda     ptr1
        add     ccount
        sta     ccount
        txa
        adc     ccount+1
@Done:  sta     ccount+1
        jmp     incsp6          ; Drop stackframe


; ----------------------------------------------------------------------------
; vfprintf - formatted output
;
; int fastcall vfprintf(FILE* f, const char* format, va_list ap)
; {
;     static struct outdesc d = {
;         0,
;         out
;     };
;
;     /* Setup descriptor */
;     d.ccount = 0;
;     d.ptr  = f;
;
;     /* Do formatting and output */
;     _printf (&d, format, ap);
;
;     /* Return bytes written */
;     return d.ccount;
; }
;
_vfprintf:
        pha                     ; Save low byte of ap

; Setup the outdesc structure

        lda     #0
        sta     ccount
        sta     ccount+1        ; Clear character-count

; Reorder the stack. Replace f on the stack by &d, so the stack frame is
; exactly as _printf expects it. Parameters will get dropped by _printf.

        ldy     #2
        lda     (c_sp),y        ; Low byte of f
        sta     ptr
        lda     #<outdesc
        sta     (c_sp),y
        iny
        lda     (c_sp),y        ; High byte of f
        sta     ptr+1
        lda     #>outdesc
        sta     (c_sp),y

; Restore low byte of ap and call _printf

        pla
        jsr     __printf

; Return the number of bytes written

        lda     ccount
        ldx     ccount+1
        rts


