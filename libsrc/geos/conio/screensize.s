
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; void screensize (unsigned char* x, unsigned char* y);
;

    	.export	 	_screensize

	.import	 	popax
	.importzp	ptr1, ptr2
	.import		xsize, ysize
	
	.include 	"../inc/geossym.inc"

_screensize:

	sta	ptr1		; Store the y pointer
	stx	ptr1+1

	jsr	popax		; get the x pointer
	sta	ptr2
	stx	ptr2+1

	lda	xsize
	sta	(ptr2),y
	lda	ysize
	sta	(ptr1),y
   	rts
