;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

	.export		_cgetc
	.import		cursor

	.include	"c128.inc"

_cgetc:	lda	KEY_COUNT 	; Get number of characters
	bne	L2	  	; Jump if there are already chars waiting

; Switch on the cursor if needed

	lda	cursor
       	beq	L1
	jsr	CURS_ON
	jmp	L2
L1:    	lda	#$01
	jsr	CURS_OFF
L2:    	lda	KEY_COUNT	; Check characters again
	beq	L2
	jsr	CURS_OFF	; Switch cursor off, if characters available

       	jsr	KBDREAD		; Read char and return in A
	ldx	#0
	rts

	   
