;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from int to long
;

       	.export	      	tosulong, toslong
	.import		decsp2
	.importzp     	sp

; Convert TOS from int to long

tosulong:
       	pha
    	jsr	decsp2		; Make room
	ldy    	#2
	lda	(sp),y
	ldy	#0
       	sta	(sp),y
	ldy    	#3
	lda    	(sp),y
	ldy	#1
	sta	(sp),y
	lda    	#0    		; Zero extend
toslong2:
	iny
	sta	(sp),y
	iny
	sta	(sp),y
	pla
	rts

toslong:
       	pha
    	jsr	decsp2		; Make room
	ldy    	#2
	lda	(sp),y
	ldy	#0		   
       	sta	(sp),y
	ldy    	#3
	lda    	(sp),y
	bmi	toslong1
	ldy	#1
	sta	(sp),y
	lda	#$00  		; Positive, high word is zero
	bne	toslong2
toslong1:
	ldy	#1
	sta	(sp),y
	lda	#$FF
	bne	toslong2

