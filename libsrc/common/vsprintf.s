;
; int vsprintf (char* Buf, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 1.12.2000
;

   	.export	      	_vsprintf
	.import	      	pushax, popax
	.import	      	_memcpy, __printf
	.importzp     	sp, ptr1

	.macpack      	generic


.data

; ----------------------------------------------------------------------------
;
; Static data for the _vsprintf routine
;

outdesc:		 	; Static outdesc structure
	.word	0	 	; ccount
	.word	out	 	; Output function pointer
	.word	0	 	; ptr
	.word	$7FFF	 	; Buffer size (max int)

.code

; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; static void out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
; {
;     /* String - be sure to check the size */
;     while (count-- && d->ccount < d->uns) {
;  	  ((char*) d->ptr) [d->ccount] = *buf;
;  	  ++buf;
;  	  ++d->ccount;
;     }
; }
;
; The assembler version assumes that the buffer is big enough and just copies
; all characters into the buffer. This has to be changed for a vsnprintf like
; function but is ok for now.
; For simplicity, we will use memcpy to copy the stuff and will just rearrange
; the stack to create a matching frame for memcpy.

out:	ldy    	#4     	       	 	; d on stack
	lda	(sp),y
	sta	ptr1
	iny
	lda	(sp),y
	sta	ptr1+1	       	 	; Save d into ptr1

; Get a pointer to the target buffer and store it into the stack frame
; currently occupied by d

	dey	  	       	 	; d->ptr
	lda	(ptr1),y       	 	; Low byte of d->ptr
	ldy	#0
	add	(ptr1),y       	 	; Low byte of d->ccount
	ldy	#4
	sta	(sp),y	       	 	; Store into d stackframe
	iny
	lda	(ptr1),y       	 	; High byte of d->ptr
	ldy	#1
	adc	(ptr1),y       	 	; High byte of d->ccount
	ldy	#5
	sta	(sp),y

; Increment the total count by the number if bytes in this chunk. While doing
; so, load count into a/x since _memcpy is a fastcall function

	jsr	popax		 	; Get count
	pha	  		 	; Save low byte
	ldy	#0
       	add	(ptr1),y	 	; Low byte of d->ccount
	sta	(ptr1),y
	iny
	txa	  		 	; High byte of count
	adc	(ptr1),y	 	; High byte of d->ccount
	sta	(ptr1),y
	pla	  		 	; Restore low byte of count

; We have the correct stackframe for memcpy now, call it

	jmp	_memcpy


; ----------------------------------------------------------------------------
; vsprintf - formatted output into a buffer
;
; int vsprintf (char* buf, const char* format, va_list ap)
; {
;     struct outdesc d;
;
;     /* Setup descriptor */
;     d.fout = out;
;     d.ptr  = buf;
;     d.uns  = 0x7FFF;
;
;     /* Do formatting and output */
;     _printf (&d, format, ap);
;
;     /* Terminate the result string */
;     buf [d.ccount++] = '\0';
;
;     /* Return bytes written */
;     return d.ccount;
; }


_vsprintf:
	pha	  		; Save low byte of ap

; Setup the outdesc structure

	lda	#0
	sta	outdesc
	sta	outdesc+1	; Clear ccount

; Reorder the stack. Replace buf on the stack by &d, so the stack frame is
; exactly as _printf expects it. Parameters will get dropped by _printf.

	ldy	#2		;
	lda	(sp),y		; Low byte of buf
	sta	outdesc+4	; Store into outdesc.ptr
	lda	#<outdesc
	sta	(sp),y
	iny
	lda	(sp),y	 	; High byte of buf
	sta	outdesc+5
	lda	#>outdesc
	sta	(sp),y

; Restore low byte of ap and call _printf

	pla
	jsr	__printf

; Terminate the string

	lda	outdesc+4	; buf
	add	outdesc+0	; +ccount
	sta	ptr1
	lda	outdesc+5
	adc	outdesc+1
	sta	ptr1+1
	ldy	#0
	tya
	sta	(ptr1),y

; Return the number of bytes written.

        lda     outdesc         ; ccount
        ldx     outdesc+1
	rts


