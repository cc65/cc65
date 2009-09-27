;
; int vsnprintf (char* Buf, size_t size, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 2009-09-26
;

   	.export	      	_vsnprintf
 	.import	      	ldaxysp, popax, incsp2, incsp6
	.import	      	_memcpy, __printf
	.importzp     	sp, ptr1

	.macpack      	generic

.data

; ----------------------------------------------------------------------------
;
; Static data for the _vsnprintf routine
;

outdesc:		 	; Static outdesc structure
	.word	0	 	; ccount
	.word	out	 	; Output function pointer
	.word	0	 	; ptr
       	.word  	0               ; Buffer size

.code

; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; static void out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
;
; Since we know, we're called with a pointer to our static outdesc structure,
; we don't need the pointer passed on the stack.

out:

; Calculate the space left in the buffer. If no space is left, don't copy
; any characters

        lda     outdesc+6               ; Low byte of buffer size
        sec
        sbc     outdesc+0               ; Low byte of bytes already written
        sta     ptr1
        lda     outdesc+7
        sbc     outdesc+1
        sta     ptr1+1
        bcs     @L0                     ; Space left
        lda     #0
        sta     ptr1
        sta     ptr1+1

; Replace the pointer to d by a pointer to the write position in the buffer
; for the call to memcpy that follows.

@L0:    lda     outdesc+0
        clc
        adc     outdesc+4
        ldy     #4
        sta     (sp),y

        lda     outdesc+1
        adc     outdesc+5
        iny
        sta     (sp),y

; Get Count from stack

        jsr     popax

; outdesc.ccount += Count;

        pha
        clc
        adc     outdesc+0
        sta     outdesc+0
        txa
        adc     outdesc+1
        sta     outdesc+1
        pla

; if (Count > Left) Count = Left;

        cmp     ptr1
        bne     @L1
        cpx     ptr1+1
@L1:    bcs     @L2
        lda     ptr1
        ldx     ptr1+1

; Jump to memcpy, which will cleanup the stack and return to the caller

@L2:    jmp     _memcpy


; ----------------------------------------------------------------------------
; vsprintf - formatted output into a buffer
;
; int __fastcall__ vsnprintf (char* buf, size_t size, const char* format, va_list ap);
;

_vsnprintf:
    	pha	   		; Save low byte of ap

; Setup the outdesc structure

    	lda	#0
    	sta	outdesc
    	sta	outdesc+1	; Clear ccount

; Get the size parameter and replace it by a pointer to outdesc. This is to
; build a stack frame for the call to _printf.
; If size is zero, there's nothing to do.

        ldy     #2
        lda     (sp),y
        sta     ptr1
        lda     #<outdesc
        sta     (sp),y
        iny
        lda     (sp),y
        sta     ptr1+1
        ora     ptr1
        beq     L9

        lda     #>outdesc
        sta     (sp),y

; Write size-1 to the outdesc structure

        ldx     ptr1
        ldy     ptr1+1
        dex
        bne     L1
        dey
L1:     stx     outdesc+6
        sty     outdesc+7

; Copy buf to the outdesc structure

        ldy     #5
        jsr     ldaxysp
        sta     outdesc+4
        stx     outdesc+5

; Restore low byte of ap and call _printf

	pla
	jsr	__printf

; Terminate the string

        lda     outdesc+0
        ldx     outdesc+1
        cpx     outdesc+7
        bne     L2
        cmp     outdesc+6
L2:     bcc     L3
        lda     outdesc+6
        ldx     outdesc+7
        clc
L3:     adc     outdesc+4
        sta     ptr1
        txa
        adc     outdesc+5
        sta     ptr1+1

        lda     #0
        tay
        sta     (ptr1),y

; Return the number of bytes written and drop buf

        lda     outdesc         ; ccount
        ldx     outdesc+1
	jmp     incsp2

; Bail out if size is zero

L9:     lda     #0
        tax
        jmp     incsp6          ; Drop parameters


