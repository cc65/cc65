
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void screensize (unsigned char* x, unsigned char* y);
;

    	.export	 	_screensize

	.import	 	popax
	.importzp	ptr1, ptr2
	
	.include 	"../inc/geossym.inc"

_screensize:

	sta	ptr1		; Store the y pointer
	stx	ptr1+1

	jsr	popax		; get the x pointer
	sta	ptr2
	stx	ptr2+1

	ldy	#0
	lda	graphMode
	bpl	L1
	lda	#80		; 80 columns (more or less)
	.byte	$2c
L1:	lda	#40		; 40 columns (more or less)
	sta	(ptr2),y
	lda	#24		; something like that for Y size
	sta	(ptr1),y
   	rts
