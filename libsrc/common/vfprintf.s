;
; int vfprintf (FILE* f, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 1.12.2000
;

	.export	      	_vfprintf
	.import	       	pushax, popax, push1, pushwysp, ldaxysp, ldaxidx, incsp6
	.import	       	_fwrite, __printf
	.importzp      	sp, ptr1, ptr2

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
;      	fwrite (buf, 1, count, (FILE*) d->ptr);
;
; Since Buf and Count are already in place, we will just push the last
; two parameters. The fwrite function will remove Buf and Count on exit.

        ldy     #5
        jsr     pushwysp        ; Push buf
        jsr     push1           ; Push #1
        ldy     #7
        jsr     pushwysp        ; Push count
        ldy     #11             ; Current offset of D
        jsr     ldaxysp         ; Load D
        ldy     #5              ; Offset of ptr1+1 in struct outdesc
        jsr     ldaxidx         ; Load
  	jsr	_fwrite
       	sta	ptr2 		; Save function result
	stx	ptr2+1

; Get D and store it in ptr1

        ldy     #5
        jsr     ldaxysp
        sta     ptr1
        stx     ptr1+1

; Load the offset of ccount in struct outdesc

	ldy	#$00

; Check the return code. Checking the high byte against $FF is ok here.

       	lda	ptr2+1
 	cmp	#$FF
	bne	@Ok

; We had an error. Store -1 into d->ccount

	sta	(ptr1),y
        iny
        bne     @Done           ; Branch always

; Result was ok, count bytes written

@Ok:	lda	(ptr1),y
	add	ptr2
	sta	(ptr1),y
        iny
	lda	(ptr1),y
	adc	ptr2+1
@Done:  sta	(ptr1),y
	jmp     incsp6          ; Drop stackframe


; ----------------------------------------------------------------------------
; vfprintf - formatted output
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


