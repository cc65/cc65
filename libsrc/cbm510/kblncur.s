;
; Ullrich von Bassewitz, 16.09.2001
;

	.export		k_blncur

	.include   	"zeropage.inc"


; ------------------------------------------------------------------------
; Blink the cursor in the interrupt

.proc   k_blncur

        lda     CURS_FLAG		; Is the cursor on?
        bne     curend			; Jump if not
        dec     CURS_BLINK
        bne     curend

; Re-initialize the blink counter

        lda     #20	  	 	; Initial value
        sta     CURS_BLINK

; Switch to the system bank, load Y with the cursor X coordinate

      	lda	#$0F
      	sta	IndReg	   	       	; Access system bank
      	ldy	CURS_X

; Check if the cursor state was on or off before

      	lda	CURS_COLOR	       	; Load color behind cursor
      	lsr	CURS_STATE	       	; Cursor currently displayed?
      	bcs    	curset		       	; Jump if yes

; Cursor was off before, switch it on

      	inc	CURS_STATE	       	; Mark as displayed
      	lda	(CRAM_PTR),y	       	; Get color behind cursor...
      	sta	CURS_COLOR	       	; ...and remember it
      	lda	CHARCOLOR	       	; Use character color

; Set the cursor with color in A

curset:	sta	(CRAM_PTR),y	       	; Store cursor color
      	lda	ExecReg
      	sta	IndReg 		       	; Switch to our segment
      	lda	(SCREEN_PTR),y
      	eor	#$80	      	       	; Toggle reverse flag
      	sta	(SCREEN_PTR),y
curend:	rts

.endproc


