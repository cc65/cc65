;
; int vprintf (FILE* f, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 1.12.2000
;

	.export	      	_vfprintf
	.import	      	pushax, popax, push1
	.import	      	_fwrite, __printf
	.importzp     	sp, ptr1, ptr2

	.macpack      	generic


.data

; ----------------------------------------------------------------------------
;
; Static data for the _vfprintf routine
;

outdesc:			; Static outdesc structure
	.word	0		; ccount
	.word	out		; Output function pointer
	.word	0		; ptr
	.word	0		; uns

.code

; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; static void out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
; {
;     /* Write to the file */
;     if (fwrite (buf, count, 1, (FILE*) d->ptr) == -1) {
;         d->ccount = -1;
;     } else {
;         d->ccount += count;
;     }
; }

out:

; About to call
;
; 	fwrite (buf, count, 1, (FILE*) d->ptr);
;
; Since Buf and Count are already in place, we will just push the last
; two parameters. The fwrite function will remove Buf and Count on exit.

  	jsr	push1
  	ldy    	#7     	       	; Offset of D+1 on stack
  	lda	(sp),y
  	sta	ptr1+1
  	dey	   		; Offset of D on stack (6)
  	lda	(sp),y
  	sta	ptr1			       
  	dey	   		; Offset of ptr+1 in struct outdesc (5)
  	lda	(ptr1),y
  	tax
  	dey
  	lda	(ptr1),y
  	jsr	pushax		; Push D->ptr
  	jsr	_fwrite
       	sta	ptr2		; Save function result
	stx	ptr2+1

; Pop the last parameter from stack and store it in ptr1. This means that
; the stack is clean now.

	jsr	popax
	sta	ptr1
	stx	ptr1+1

; Load the offset of ccount in struct outdesc

	ldy	#$00

; Check the return code. Checking the hig byte against $FF is ok here.

       	lda	ptr2+1
 	cmp	#$FF
	bne	@Ok

; We had an error. Store -1 into d->ccount

	sta	(ptr1),y
	iny
	sta	(ptr1),y
	rts

; Result was ok, count bytes written

@Ok:	lda	(ptr1),y
	add	ptr1
	sta	(ptr1),y
	iny
	lda	(ptr1),y
	adc	ptr1+1
	sta	(ptr1),y
	rts


; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; int vfprintf (FILE* f, const char* format, va_list ap)
; {
;     struct outdesc d;
;
;     /* Setup descriptor */
;     d.fout = out;
;     d.ptr  = f;
;
;     /* Do formatting and output */
;     _printf (&d, format, ap);
;
;     /* Return bytes written */
;     return d.ccount;
; }

_vfprintf:
	pha			; Save low byte of ap

; Setup the outdesc structure

	lda	#0
	sta	outdesc
	sta	outdesc+1	; Clear ccount

; Reorder the stack. Replace f on the stack by &d, so the stack frame is
; exactly as _printf expects it. Parameters will get dropped by _printf.

	ldy	#2		;
	lda	(sp),y		; Low byte of f
	sta	outdesc+4	;
	lda	#<outdesc
	sta	(sp),y
	iny
	lda	(sp),y	 	; High byte of f
	sta	outdesc+5
	lda	#>outdesc
	sta	(sp),y

; Restore low byte of ap and call _printf

	pla
	jsr	__printf

; Return the number of bytes written

	lda	outdesc
	ldx	outdesc+1
	rts


