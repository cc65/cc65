;
; Ullrich von Bassewitz, 24.04.1999
;
; Routines for the 1351 proportional mouse. Parts of the code are from
; the Commodore 1351 mouse users guide.
;

	.export	    	_mouse_init, _mouse_done
	.export	    	_mouse_hide, _mouse_show
	.export	    	_mouse_box, _mouse_info
	.export	    	_mouse_move

       	.import	       	popa, popsreg, addysp1
	.importzp   	sp, sreg

	.include    	"c64.inc"

.code

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_init (unsigned char port, unsigned char sprite);
;

_mouse_init:
	tax				; Save sprite number
	jsr	popa			; Get the port number

       	ldy    	OldIRQ+1		; Already initialized?
       	bne    	Done			; Jump if yes

	stx	MouseSprite		; Remember the sprite number
	sta	MousePort		; Remember the port number

; Initialize variables

       	ldx 	#0
	stx	XPos
	stx	XPos+1
	stx	YPos
	stx	YPos+1
	stx	OldPotX
	stx	OldPotY
       	stx    	XMin
 	stx	XMin+1
	stx	YMin
	stx	YMin+1
	stx	YMax+1
	inx	      			; X = 1
       	stx	Visible			; Mouse *not* visible
	stx	XMax+1			; >320
	ldx	#<320
	stx	XMax
	ldx	#200
	stx	YMax

; Remember the old IRQ vector

     	lda    	IRQVec
     	sta   	OldIRQ
     	lda   	IRQVec+1
     	sta   	OldIRQ+1

; Set our own IRQ vector

     	lda    	#<MouseIRQ
     	ldx   	#>MouseIRQ
     	bne   	SetIRQ

; --------------------------------------------------------------------------
;
; void mouse_done (void);
;

_mouse_done:
       	lda    	OldIRQ	  		; Initialized?
     	ldx	OldIRQ+1
     	beq    	Done  	  		; Jump if no
	ldy	#0
	sty	OldIRQ+1  		; Reset the initialized flag
SetIRQ:	sei		  		; Disable interrupts
      	sta	IRQVec	  		; Set the new/old vector
      	stx	IRQVec+1
	cli				; Enable interrupts
Done: 	rts

; --------------------------------------------------------------------------
;
; void mouse_hide (void);
;

_mouse_hide:
       	lda	Visible			; Get the flag
	bne	@L1			; Jump if already invisible
       	ldx	MouseSprite		; Sprite defined?
	beq	@L1			; Jump if no

	lda	BitMask-1,x		; Get bit mask
	eor	#$FF			; We must clear the bit

  	sei				; Disable interrupts
	and	VIC_SPR_ENA
	sta	VIC_SPR_ENA	     	; Disable sprite
	cli				; Enable interrupts

@L1:	inc	Visible			; Set the flag to invisible
	rts

; --------------------------------------------------------------------------
;
; void mouse_show (void);
;

_mouse_show:
       	dec	Visible			; Get the flag
	bne	@L1			; Jump if still invisible
       	ldx	MouseSprite		; Sprite defined?
	beq	@L1			; Jump if no

	lda	BitMask-1,x		; Get bit mask
       	sei				; Disable interrupts
       	ora	VIC_SPR_ENA
	sta	VIC_SPR_ENA		; Enable sprite
	cli				; Enable interrupts

@L1:	rts

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
;

_mouse_box:
	sei	    			; Disable interrupts

	sta	YMax
	stx	YMax+1			; maxy

	ldy	#0
	lda	(sp),y
	sta	XMax
	iny
	lda	(sp),y
	sta	XMax+1			; maxx

	iny
	lda	(sp),y
	sta	YMin
	iny
	lda	(sp),y
	sta	YMin+1			; miny

	iny
	lda	(sp),y
	sta	XMin
	iny
	lda	(sp),y
	sta	XMin+1			; minx

	cli	      			; Enable interrupts

	jmp	addysp1			; Drop params, return

; --------------------------------------------------------------------------
;
; void mouse_info (...);
;

_mouse_info:
      	rts


; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_move (int x, int y);
;

_mouse_move:
	jsr	popsreg			; Get X
	sei				; Disable interrupts

	sta	YPos
	stx	YPos+1
	lda	sreg
	ldx	sreg+1
	sta	XPos
	stx	XPos+1			; Set new position

	lda	Visible			; Mouse visible?
	bne	@L9			; Jump if no
	lda	MouseSprite		; Sprite defined?
	beq	@L9

	jsr	MoveSprite		; Move the sprite to the mouse pos

@L9:	cli				; Enable interrupts
	rts

; --------------------------------------------------------------------------
;
; Mouse interrupt handler
;

MouseIRQ:
	cld
	lda	SID_ADConv1		; Get mouse X movement
	ldy	OldPotX
	jsr	MoveCheck  		; Calculate movement vector
	sty	OldPotX

; Calculate the new X coordinate (--> a/y)

 	clc
	adc	XPos
	tay				; Remember low byte
	txa
	adc	XPos+1

; Limit the X coordinate to the bounding box

	cpy    	XMin+1
	bne	@L1
	cmp	XMin
@L1:	bpl	@L2
       	ldy	XMin
    	lda	XMin+1
    	jmp	@L4

@L2:	cpy	XMax+1
	bne	@L3
	cmp	XMax
	beq	@L4
@L3:	bmi	@L4
       	ldy    	XMax
	lda	XMax+1
@L4:	sty	XPos
    	sta	XPos+1

; Calculate the Y movement vector

	lda	SID_ADConv2	 	; Get mouse Y movement
	ldy	OldPotY
	jsr	MoveCheck	 	; Calculate movement
	sty	OldPotY

; Calculate the new Y coordinate (--> a/y)

	clc
	adc	YPos
	tay	    			; Remember low byte
	txa
	adc	YPos+1

; Limit the Y coordinate to the bounding box

       	cpy    	YMin+1
	bne	@L5
    	cmp	YMin
@L5:	bpl	@L6
       	ldy	YMin
    	lda	YMin+1
    	jmp	@L8

@L6:	cpy	YMax+1
    	bne	@L7
    	cmp	YMax
    	beq	@L8
@L7:	bmi	@L8
       	ldy    	YMax
    	lda	YMax+1
@L8:   	sty	YPos
	sta	YPos+1

; Jump to the next IRQ handler

  	jmp	(OldIRQ)


; --------------------------------------------------------------------------
;
; Move check routine, called for both coordinates.
;
; Entry:	y = old value of pot register
;     		a = current value of pot register
; Exit:		y = value to use for old value
;     		x/a = delta value for position
;

MoveCheck:
      	sty	OldValue
      	sta	NewValue
      	ldx	#$00

      	sec	    			; a = mod64 (new - old)
      	sbc	OldValue
      	and	#%01111111
      	cmp	#%01000000		; if (a > 0)
      	bcs	@L1 			;
      	lsr	a   			;   a /= 2;
      	beq	@L2 			;   if (a != 0)
      	ldy	NewValue     		;     y = NewValue
      	rts	    			;   return

@L1:  	ora	#%11000000		; else or in high order bits
      	cmp	#$FF			; if (a != -1)
      	beq	@L2
      	sec
      	ror	a   			;   a /= 2
      	ldx	#$FF			;   high byte = -1
      	ldy	NewValue
      	rts

@L2:  	lda	#0
      	rts

; --------------------------------------------------------------------------
;
; Move the mouse sprite to the current mouse position. Must be called
; with interrupts off.
;

MoveSprite:
	lda	Visible			; Mouse pointer visible?
	bne	@L9    			; Jump if no
	ldx    	MouseSprite    	       	; Sprite defined?
	beq	@L9    			; Jump if no
	ldy	BitMask-1,x		; Get high bit mask
	txa
 	asl	a   			; Index*2
	tax

; Set the X position

	lda    	XPos+1 	       	       	; Negative?
       	bmi	@L2    			; Jump if yes
	beq	@L1
       	tya				; Load high position bit
@L1:	ora	VIC_SPR_HI_X		; Set high bit
	sta	VIC_SPR_HI_X
	lda	XPos
	sta	VIC_SPR0_X,x		; Set low byte

; Set the Y position

@L2:	ldy	YPos+1			; Negative or too large?
	bne	@L9			; Jump if yes
	lda	YPos
	sta	VIC_SPR0_Y,x		; Set Y position

; Done

@L9:  	rts

; --------------------------------------------------------------------------
; Data

.bss

OldIRQ:	       	.res   	2		; Old IRQ vector
MousePort:	.res	1		; Port used for the mouse
MouseSprite:	.res   	1		; Number of sprite to control
OldValue:	.res   	1		; Temp for MoveCheck routine
NewValue:	.res   	1		; Temp for MoveCheck routine

Visible:	.res   	1		; Is the mouse visible?
OldPotX:	.res   	1		; Old hw counter values
OldPotY:	.res   	1

XPos:		.res   	2		; Current mouse position, X
YPos:		.res   	2		; Current mouse position, Y

XMin:		.res	2		; X1 value of bounding box
YMin:		.res	2		; Y1 value of bounding box
XMax:		.res	2		; X2 value of bounding box
YMax:		.res	2		; Y2 value of bounding box

.data

BitMask:	.byte  	$01, $02, $04, $08, $10, $20, $40, $80




		    
