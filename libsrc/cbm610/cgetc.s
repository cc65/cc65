;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

	.export	   	_cgetc
	.import	   	plot, write_crtc, sys_bank, restore_bank
	.import	   	cursor
        .import         sysp0: zp, sysp3: zp

	.include   	"cbm610.inc"


_cgetc:	lda     IndReg
        pha
        lda     #$0F
        sta     IndReg          ; Switch to the system bank

        ldy     #$D1
        lda     (sysp0),y       ; Get number of keys in keyboard buffer
	bne	L2 	  	; Jump if there are already chars waiting

; Switch on the cursor if needed

       	lda	cursor
       	beq    	L0 	    	; Jump if no cursor

       	jsr	plot		; Set the current cursor position
        ldy     #$D4
        lda     (sysp0),y       ; Get the cursor format
       	ldy	#10
       	jsr	write_crtc	; Set the cursor format

L0:     ldy     #$D1
L1:     lda     (sysp0),y       ; Get the number of keys in the buffer
       	beq	L1              ; Wait until we have some keys

       	ldy	#10
       	lda	#$20		; Cursor off
       	jsr	write_crtc

L2:     ldy     #$D1
        lda     (sysp0),y       ; Get number of chars in buffer
        tax
        ldy     #$AB
        lda     (sysp3),y       ; Get first char from keyboard buffer
        sta     c               ; Save for return
        dex
        txa
        ldy     #$D1
        sta     (sysp0),y
        sei
        jmp     L4
L3:     iny
        lda     (sysp3),y
        dey
        sta     (sysp3),y
        iny
L4:     dex
        bpl     L3       
        cli

        pla
        sta     IndReg          ; Restore old segment

       	ldx	#$00		; High byte
       	lda     c               ; First char from buffer
       	rts

.bss

c:      .byte   0
