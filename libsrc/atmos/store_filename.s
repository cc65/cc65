	.export		store_filename
	.importzp	ptr1

store_filename:
	sta	ptr1
	stx	ptr1+1
	ldy	#$0f   ;store filename
:	lda	(ptr1),y
	sta	$027f,y
	dey
	bpl	:-
	rts
