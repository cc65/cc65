;
; Ullrich von Bassewitz, 16.09.2001
;
; char cgetc (void);
;

	.export	   	_cgetc
	.import	   	cursor

	.include   	"zeropage.inc"
	.include	"page3.inc"


; ------------------------------------------------------------------------

.proc	_cgetc

    	lda	KeyIndex		; Characters waiting?
       	bne    	L3			; Jump if so

; Switch on the cursor if needed

  	lda	CURS_FLAG
  	pha
  	lda	cursor
  	jsr	setcursor
L1:	lda	KeyIndex
  	beq	L1
  	ldx	#0
    	pla
 	bne	L2
       	inx
L2:    	txa
       	jsr	setcursor

; Read the character from the keyboard buffer

L3:    	ldx    	#$00		; Get index
       	ldy    	KeyBuf		; Get first character in the buffer
       	sei
L4:    	lda    	KeyBuf+1,x	; Move up the remaining chars
       	sta    	KeyBuf,x
       	inx
       	cpx    	KeyIndex
       	bne    	L4
       	dec    	KeyIndex
       	cli

       	ldx    	#$00		; High byte
       	tya    			; First char from buffer
       	rts

.endproc

; ------------------------------------------------------------------------
;

.proc	setcursor

	ldy	#$00		       	;
       	tax			       	; On or off?
       	bne    	@L9		       	; Go set it on
    	lda	CURS_FLAG	       	; Is the cursor currently off?
       	bne    	@L8			; Jump if yes
    	lda	#1
    	sta	CURS_FLAG		; Mark it as off
    	lda	CURS_STATE		; Cursor currently displayed?
	sty	CURS_STATE		; Cursor will be cleared later
       	beq    	@L8			; Jump if no

; Switch to the system bank, load Y with the cursor X coordinate

	lda	#$0F
	sta	IndReg	   	       	; Access system bank
	ldy	CURS_X

; Reset the current cursor

     	lda    	CURS_COLOR
	sta	(CRAM_PTR),y	       	; Store cursor color
      	lda	ExecReg
      	sta	IndReg 		       	; Switch to our segment
      	lda	(SCREEN_PTR),y
      	eor	#$80	      	       	; Toggle reverse flag
      	sta	(SCREEN_PTR),y

; Done

@L8:  	rts

@L9: 	sty	CURS_FLAG	       	; Cursor on (Y = 0)
	rts

.endproc

