;
; int __fastcall__ vsnprintf (char* Buf, size_t size, const char* Format, va_list ap);
;
; 2009-09-26, Ullrich von Bassewitz
; 2015-07-17, Greg King
;

        .export         _vsnprintf, vsnprintf
        .import         ldaxysp, popax, incsp2, incsp6
        .import         _memcpy, __printf
        .importzp       sp, ptr1

        .include        "errno.inc"

        .macpack        generic

.data

; ----------------------------------------------------------------------------
;
; Static data for the _vsnprintf routine
;

outdesc:                        ; Static outdesc structure
ccount: .word   0               ; ccount
func:   .word   out             ; Output function pointer
bufptr: .word   0               ; ptr
bufsize:.word   0               ; Buffer size

.code

; ----------------------------------------------------------------------------
; vsprintf - formatted output into a buffer
;
; int __fastcall__ vsnprintf (char* buf, size_t size, const char* format, va_list ap);
;

_vsnprintf:
        pha                     ; Save ap
        txa
        pha

; Setup the outdesc structure. This is also an additional entry point for
; vsprintf with ap on stack

vsnprintf:
        lda     #0
        sta     ccount+0
        sta     ccount+1        ; Clear ccount

; Get the size parameter and replace it by a pointer to outdesc. This is to
; build a stack frame for the call to _printf. The size must not be greater
; than INT_MAX because the return type is int. If the size is zero,
; then nothing will be written into the buffer; but, the arguments still will
; be formatted and counted.

        ldy     #2
        lda     (sp),y
        sta     ptr1

        lda     #<outdesc
        sta     (sp),y

        iny
        lda     (sp),y
        bmi     L9              ; More than $7FFF
        sta     ptr1+1

        lda     #>outdesc
        sta     (sp),y

; Write size-1 to outdesc.uns.  It will be -1 if there is no buffer.

        ldy     ptr1+1
        ldx     ptr1
        bne     L1
        dey
L1:     dex
        stx     bufsize+0
        sty     bufsize+1

; Copy buf to the outdesc.ptr

        ldy     #5
        jsr     ldaxysp
        sta     bufptr+0
        stx     bufptr+1

; There must be a buffer if its size is non-zero.

        bit     bufsize+1
        bmi     L5
        ora     bufptr+1
        bze     L0              ; The pointer shouldn't be NULL

; Restore ap and call _printf

L5:     pla
        tax
        pla
        jsr     __printf

; Terminate the string if there is a buffer.  The last char. is at either
; bufptr+bufsize or bufptr+ccount, whichever is smaller.

        ldx     bufsize+1
        bmi     L4              ; -1 -- No buffer
        lda     bufsize+0
        cpx     ccount+1
        bne     L2
        cmp     ccount+0
L2:     bcc     L3
        lda     ccount+0
        ldx     ccount+1
        clc
L3:     adc     bufptr+0
        sta     ptr1
        txa
        adc     bufptr+1
        sta     ptr1+1

        lda     #0
        tay
        sta     (ptr1),y

; Return the number of bytes written and drop buf

L4:     lda     ccount+0
        ldx     ccount+1
        jmp     incsp2

; Bail out if size is too high.

L9:     ldy     #ERANGE
        .byte   $2C             ;(bit $xxxx)

; NULL buffer pointers usually are invalid.

L0:     ldy     #EINVAL
        pla                     ; Drop ap
        pla
        tya
        jsr     __directerrno   ; Return -1
        jmp     incsp6          ; Drop parameters


; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; static void __cdecl__ out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
;
; Since we know, we're called with a pointer to our static outdesc structure,
; we don't need the pointer passed on the stack.

out:

; Calculate the space left in the buffer. If no space is left, don't copy
; any characters

        lda     bufsize+0               ; Low byte of buffer size
        sec
        sbc     ccount+0                ; Low byte of bytes already written
        sta     ptr1
        lda     bufsize+1
        bmi     @L9                     ; -1 -- No buffer
        sbc     ccount+1
        sta     ptr1+1
        bcs     @L0                     ; Branch if space left
@L9:    lda     #$0000
        sta     ptr1
        sta     ptr1+1                  ; No space left

; Replace the pointer to d by a pointer to the write position in the buffer
; for the call to memcpy that follows.

@L0:    lda     bufptr+0
        clc
        adc     ccount+0
        ldy     #4
        sta     (sp),y

        lda     bufptr+1
        adc     ccount+1
        iny
        sta     (sp),y

; Get Count from stack

        jsr     popax

; outdesc.ccount += Count;

        pha
        clc
        adc     ccount+0
        sta     ccount+0
        txa
        adc     ccount+1
        sta     ccount+1
        pla

; if (Count > Left) Count = Left;

        cpx     ptr1+1
        bne     @L1
        cmp     ptr1
@L1:    bcc     @L2
        lda     ptr1
        ldx     ptr1+1

; Jump to memcpy, which will cleanup the stack and return to the caller

@L2:    jmp     _memcpy



