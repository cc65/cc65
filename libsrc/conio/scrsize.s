;
; Ullrich von Bassewitz, 08.08.1998
;
; void screensize (unsigned char* x, unsigned char* y);
;

    	.export	 	_screensize

	.import	 	popax
	.import		xsize, ysize
	.importzp	ptr1, ptr2

.proc	_screensize

	sta	ptr1		; Store the y pointer
	stx	ptr1+1

	jsr	popax		; get the x pointer
	sta	ptr2
	stx	ptr2+1

	ldy	#0
	lda	xsize
	sta	(ptr2),y
	lda	ysize
	sta	(ptr1),y
   	rts

.endproc

