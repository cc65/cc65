;
; Ullrich von Bassewitz, 24.04.1999
;
; Routines for the 1351 proportional mouse. Parts of the code are from
; the Commodore 1351 mouse users guide.
;

   	.export	    	_mouse_init, _mouse_done
   	.export	    	_mouse_hide, _mouse_show
   	.export	    	_mouse_box, _mouse_info
   	.export	    	_mouse_move, _mouse_pos
	.export		_mouse_buttons, _mouse_info

	.import		_readjoy
       	.import	       	popa, popsreg, addysp1
   	.importzp   	ptr1, sp, sreg

   	.include    	"c64.inc"

	.macpack	generic

.code

; --------------------------------------------------------------------------
;
; unsigned char __fastcall__ mouse_init (unsigned char port,
;					 unsigned char sprite,
;					 unsigned char type);
;

_mouse_init:
	jsr	popa			; Ignore the type, get sprite param
   	tax				; Save sprite number
   	jsr	popa			; Get the port number

       	ldy    	OldIRQ+1		; Already initialized?
       	bne    	AlreadyInitialized	; Jump if yes

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
 	stx	XMin+1			; XMin = 0
	lda	#29
	sta    	YMin
	stx	YMin+1			; YMin = 29
	lda    	#250
	sta	YMax
   	stx	YMax+1			; YMax = 250
	inx	      			; X = 1
       	stx	Visible			; Mouse *not* visible
	lda    	#<344
	sta	XMax
	stx	XMax+1			; XMax = 344

; Remember the old IRQ vector

     	lda    	IRQVec
     	sta   	OldIRQ
     	lda   	IRQVec+1
     	sta   	OldIRQ+1

; Set our own IRQ vector. We cheat here to save a few bytes of code:
; The function is expected to return a value not equal to zero on success,
; and since we know that the high byte of the IRQ handler address is never
; zweo, we will return just this byte.

     	ldx    	#<MouseIRQ
     	lda   	#>MouseIRQ
     	bne   	SetIRQ			; Branch always

AlreadyInitialized:
	lda	#0			; Error
	rts

; --------------------------------------------------------------------------
;
; void mouse_done (void);
;

_mouse_done:
       	ldx    	OldIRQ	  	      	; Initialized?
     	lda 	OldIRQ+1
     	beq    	Done   	       	      	; Jump if no
	ldy 	#0
   	sty 	OldIRQ+1  		; Reset the initialized flag
SetIRQ:	sei 		  		; Disable interrupts
      	stx 	IRQVec	  		; Set the new/old vector
      	sta 	IRQVec+1
   	cli 				; Enable interrupts
Done: 	rts

; --------------------------------------------------------------------------
;
; void mouse_hide (void);
;

_mouse_hide:
       	lda 	Visible			; Get the flag
	bne 	@L1			; Jump if already invisible
       	ldx 	MouseSprite		; Sprite defined?
	beq 	@L1			; Jump if no

	lda    	NotMask-1,x		; Get clean mask

  	sei 				; Disable interrupts
	and 	VIC_SPR_ENA
	sta 	VIC_SPR_ENA	     	; Disable sprite
	cli 				; Enable interrupts

@L1:	inc 	Visible			; Set the flag to invisible
	rts

; --------------------------------------------------------------------------
;
; void mouse_show (void);
;

_mouse_show:
	lda 	Visible			; Mouse already visible?
	beq 	@L1			; Jump if yes
       	dec 	Visible			; Get the flag
	bne 	@L1			; Jump if still invisible
       	ldx 	MouseSprite		; Sprite defined?
	beq 	@L1			; Jump if no

       	sei 				; Disable interrupts
	jsr	MoveSprite1		; Move the sprite to it's position

	ldx	MouseSprite		; Get sprite number (again)
	lda 	BitMask-1,x		; Get bit mask
       	ora 	VIC_SPR_ENA
	sta 	VIC_SPR_ENA		; Enable sprite

	cli 				; Enable interrupts

@L1:	rts

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
;

_mouse_box:
   	ldy 	#0			; Stack offset

   	php
   	sei 	    			; Disable interrupts

   	sta 	YMax
   	stx 	YMax+1			; maxy

   	lda 	(sp),y
   	sta 	XMax
   	iny
   	lda 	(sp),y
   	sta 	XMax+1			; maxx

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

   	plp	      			; Enable interrupts

   	jmp	addysp1			; Drop params, return

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_pos (struct mouse_pos* pos);
; /* Return the current mouse position */
;

_mouse_pos:
       	sta	ptr1
	stx	ptr1+1			; Remember the argument pointer

	ldy	#0			; Structure offset

	php
	sei				; Disable interrupts

	lda     XPos			; Transfer the position
	sta	(ptr1),y
	lda	XPos+1
	iny
	sta	(ptr1),y
	lda	YPos
	iny
	sta	(ptr1),y
	lda	YPos
	iny
	sta	(ptr1),y

	plp				; Restore initial interrupt state

	rts				; Done

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_info (struct mouse_info* info);
; /* Return the state of the mouse buttons and the position of the mouse */
;

_mouse_info:

; We're cheating here to keep the code smaller: The first fields of the
; mouse_info struct are identical to the mouse_pos struct, so we will just
; call _mouse_pos to initialize the struct pointer and fill the position
; fields.

        jsr	_mouse_pos

; Fill in the button state

	jsr     _mouse_buttons		; Will not touch ptr1
	ldy	#4
	sta	(ptr1),y

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

	jsr	MoveSprite		; Move the sprite to the mouse pos

@L9:	cli				; Enable interrupts
	rts


; --------------------------------------------------------------------------
;
; unsigned char mouse_buttons (void);
;

_mouse_buttons:
	lda	MousePort		; Get the port
	jmp	_readjoy		; Same as joystick

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

       	add	XPos
      	tay	      			; Remember low byte
      	txa
      	adc	XPos+1
	tax

; Limit the X coordinate to the bounding box

	cpy	XMin
	sbc	XMin+1
	bpl	@L1
       	ldy    	XMin
       	ldx	XMin+1
    	jmp	@L2
@L1:	txa

    	cpy	XMax
    	sbc	XMax+1
    	bmi	@L2
    	ldy	XMax
    	ldx	XMax+1
@L2:	sty	XPos
	stx	XPos+1

; Calculate the Y movement vector

	lda	SID_ADConv2	 	; Get mouse Y movement
	ldy	OldPotY
	jsr	MoveCheck	 	; Calculate movement
	sty	OldPotY

; Calculate the new Y coordinate (--> a/y)

      	sta	OldValue
      	lda	YPos
      	sub	OldValue
      	tay
      	stx	OldValue
      	lda	YPos+1
      	sbc	OldValue
      	tax

   	cpy	YMin
	sbc	YMin+1
	bpl	@L3
       	ldy    	YMin
       	ldx	YMin+1
    	jmp	@L4
@L3:	txa

    	cpy	YMax
    	sbc	YMax+1
    	bmi	@L4
    	ldy	YMax
    	ldx	YMax+1
@L4:	sty	YPos
	stx	YPos+1

; Move the mouse sprite if it is enabled

	jsr	MoveSprite		; Move the sprite

; Jump to the next IRQ handler

	jmp	(OldIRQ)


; --------------------------------------------------------------------------
;
; Move check routine, called for both coordinates.
;
; Entry:   	y = old value of pot register
;     	   	a = current value of pot register
; Exit:	   	y = value to use for old value
;     	   	x/a = delta value for position
;

MoveCheck:
      	sty	OldValue
      	sta	NewValue
      	ldx 	#$00

      	sub	OldValue		; a = mod64 (new - old)
      	and	#%01111111
      	cmp	#%01000000		; if (a > 0)
      	bcs	@L1 			;
      	lsr	a   			;   a /= 2;
      	beq	@L2 			;   if (a != 0)
      	ldy   	NewValue     		;     y = NewValue
      	rts   	    			;   return

@L1:  	ora   	#%11000000		; else or in high order bits
      	cmp   	#$FF			; if (a != -1)
      	beq   	@L2
      	sec
      	ror   	a   			;   a /= 2
       	dex				;   high byte = -1 (X = $FF)
      	ldy   	NewValue
      	rts

@L2:   	txa				; A = $00
      	rts

; --------------------------------------------------------------------------
;
; Move the mouse sprite to the current mouse position. Must be called
; with interrupts off. MoveSprite1 is an entry without checking and
; loading X
;

MoveSprite:

	lda   	Visible			; Mouse visible?
       	bne    	MoveSpriteDone		; Jump if no
	ldx    	MouseSprite		; Sprite defined?
	beq	MoveSpriteDone		; Jump if no

; Set the high X bit

MoveSprite1:
   	lda	VIC_SPR_HI_X		; Get high X bits of all sprites
   	and	NotMask-1,x		; Mask out sprite bit
   	ldy	XPos+1			; Test Y position
   	beq	@L1
   	ora	BitMask-1,x		; Set high X bit
@L1:	sta	VIC_SPR_HI_X		; Set hi X sprite values

; Set the low X byte

   	txa
   	asl	a   			; Index*2
   	tax
   	lda	XPos
   	sta	VIC_SPR0_X-2,x 	       	; Set low byte

; Set the Y position

   	ldy	YPos+1	      		; Negative or too large?
   	bne	MoveSpriteDone		; Jump if yes
   	lda	YPos
   	sta	VIC_SPR0_Y-2,x		; Set Y position

; Done

MoveSpriteDone:
	rts

; --------------------------------------------------------------------------
; Data

.bss

OldIRQ:	       	.res   	2		; Old IRQ vector
MousePort:	.res	1		; Port used for the mouse
MouseSprite:	.res   	1		; Number of sprite to control
OldValue:	.res   	1		; Temp for MoveCheck routine
NewValue:	.res   	1		; Temp for MoveCheck routine

Visible:	.res   	1		; Is the mouse visible?
OldPotX:   	.res   	1		; Old hw counter values
OldPotY:	.res   	1

XPos:		.res   	2		; Current mouse position, X
YPos:		.res   	2		; Current mouse position, Y

XMin:		.res	2		; X1 value of bounding box
YMin:		.res	2		; Y1 value of bounding box
XMax:		.res	2		; X2 value of bounding box
YMax:		.res	2		; Y2 value of bounding box

.data

BitMask:	.byte  	$01, $02, $04, $08, $10, $20, $40, $80
NotMask:       	.byte  	$FE, $FD, $FB, $F7, $EF, $DF, $BF, $7F





