;
; Ullrich von Bassewitz, 19.09.2001
;
; Routines for the 1351 proportional mouse. Parts of the code are from
; the Commodore 1351 mouse users guide.
;

   	.export	    	_mouse_init, _mouse_done
   	.export	    	_mouse_hide, _mouse_show
   	.export	    	_mouse_box, _mouse_info
   	.export	    	_mouse_move, _mouse_pos
	.export		_mouse_buttons, _mouse_info
	.condes		MouseIRQ, 2

	.import		_readjoy
	.import		sys_bank, restore_bank
       	.import	       	popax, addysp1
   	.importzp   	vic, sid, ptr1, sp

	.include	"zeropage.inc"
   	.include    	"io.inc"

	.macpack	generic


.code

; --------------------------------------------------------------------------
;
; Constants
;

SPRITE_HEIGHT  	= 21
SPRITE_WIDTH	= 24
SCREEN_HEIGHT	= 200
SCREEN_WIDTH	= 320
XCORR		= SPRITE_WIDTH

; --------------------------------------------------------------------------
;
; unsigned char __fastcall__ mouse_init (unsigned char type);
;

.proc	_mouse_init
       	lda    	Initialized		; Already initialized?
       	bne    	AlreadyInitialized	; Jump if yes

; Initialize variables

       	ldx 	#0
	lda	#XCORR
       	sta	XPos
	stx	XPos+1
   	stx	YPos
	stx	YPos+1
	stx	OldPotX
	stx	OldPotY
       	stx    	XMin
 	stx	XMin+1		   	; XMin = 0
	lda	#50   		   	; ## FIXME: This is the PAL value
	sta	YCorr
	sta	YPos
	stx	YPos+1
	sec
        sbc    	#SPRITE_HEIGHT		; Sprite height in pixels
	sta    	YMin
	stx	YMin+1	      		; YMin = 29
	lda    	#SCREEN_HEIGHT 		; Vertical screen res
	add	YCorr	      		; Add correction factor
	sta	YMax
   	stx	YMax+1
	inx	      	      		; X = 1
       	stx	Invisible     		; Mouse *not* visible
	lda    	#<(SCREEN_WIDTH + SPRITE_WIDTH)
	sta	XMax
	stx	XMax+1	     		; XMax = 320 + sprite width

; Mouse successfully initialized

        lda     #1
	sta	Initialized
	rts

AlreadyInitialized:
	lda	#0  			; Error
	rts

.endproc

; --------------------------------------------------------------------------
;
; void mouse_done (void);
;

_mouse_done:

   	lda 	#0
       	sta    	Initialized  		; Reset the initialized flag

; Disable the mouse sprite

DisableSprite:

  	ldx	IndReg
	lda	#$0F
	sta	IndReg			; Switch to the system bank

	ldy	#VIC_SPR_ENA
  	sei 	      			; Disable interrupts
	lda 	(vic),y
	and    	#$FE  			; Clear bit for sprite #0
	sta 	(vic),y		        ; Disable sprite
      	cli	      			; Enable interrupts

	stx     IndReg			; Switch back the segment
	rts

; --------------------------------------------------------------------------
;
; void mouse_hide (void);
;

.proc	_mouse_hide

       	lda 	Invisible		; Get the flag
	bne 	@L1			; Jump if already invisible
	jsr	DisableSprite		; Disabe the mouse sprite
@L1:	inc 	Invisible		; Set the flag to invisible
	rts

.endproc

; --------------------------------------------------------------------------
;
; void mouse_show (void);
;

.proc	_mouse_show

	lda 	Invisible		; Mouse invisible?
	beq 	@L1   			; Jump if no
       	dec 	Invisible 		; Set the flag
	bne 	@L1   	  		; Jump if still invisible

	jsr	sys_bank		; Switch to the system bank

       	sei 	      			; Disable interrupts
	jsr	MoveSprite1		; Move the sprite to it's position
	ldy	#VIC_SPR_ENA
       	lda    	(vic),y	    		; Get sprite enable register
	ora	#$01  			; Enable sprite #0
       	sta    	(vic),y			; Write back
	cli 	      			; Enable interrupts

	jsr     restore_bank		; Switch back the bank

@L1:	rts

.endproc

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
;

.proc	_mouse_box

   	ldy 	#0    			; Stack offset

    	add	YCorr 			; Adjust the Y value
    	bcc	@L1
    	inx
        clc
@L1:	sei 	      			; Disable interrupts

   	sta 	YMax
   	stx 	YMax+1	      		; maxy

      	lda 	(sp),y
	adc	#XCORR
   	sta 	XMax
   	iny
   	lda 	(sp),y
	adc	#$00
   	sta 	XMax+1	      		; maxx

   	iny
   	lda	(sp),y
 	add	YCorr
	sta	YMin
   	iny
   	lda	(sp),y
	adc	#$00
   	sta	YMin+1	      		; miny

   	iny
   	lda	(sp),y
	add	#XCORR
	sta	XMin
   	iny
   	lda	(sp),y
	adc	#$00
	sta	XMin+1	      		; minx

   	cli	      	      		; Enable interrupts

   	jmp	addysp1			; Drop params, return

.endproc

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_pos (struct mouse_pos* pos);
; /* Return the current mouse position */
;

.proc	_mouse_pos

       	sta	ptr1
	stx	ptr1+1			; Remember the argument pointer

	ldy	#0    			; Structure offset
	sec	      			; Needed for the SBC later

	sei	      			; Disable interrupts
	lda     XPos  			; Transfer the position
	sbc	#XCORR
	sta	(ptr1),y
	lda	XPos+1
	sbc	#$00
	iny
 	sta	(ptr1),y
      	lda	YPos
	ldx	YPos+1
	cli	      			; Restore initial interrupt state

	sub     YCorr 			; Apply the Y correction value
	bcs	@L1
	dex
@L1:   	iny
      	sta	(ptr1),y		; Store YPos
    	txa
    	iny
    	sta	(ptr1),y

    	rts	      			; Done

.endproc

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_info (struct mouse_info* info);
; /* Return the state of the mouse buttons and the position of the mouse */
;

.proc	_mouse_info

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

.endproc

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_move (int x, int y);
;

.proc	_mouse_move

	add     YCorr 			; Add Y coordinate correction
	bcc	@L1
	inx
	clc
@L1:	sei
      	sta	YPos
    	stx	YPos+1
	cli

	jsr	popax 			; Get X
	adc	#XCORR			; Adjust X coordinate
	bcc	@L2
	inx
@L2:	jsr	sys_bank
	sei
    	sta	XPos
    	stx	XPos+1			; Set new position
    	jsr	MoveSprite		; Move the sprite to the mouse pos
	cli	      			; Enable interrupts
	jsr     restore_bank

	rts

.endproc

; --------------------------------------------------------------------------
;
; unsigned char mouse_buttons (void);
;

.proc	_mouse_buttons

 	lda	#$00			; Use port #0
	jmp	_readjoy		; Same as joystick

.endproc


; --------------------------------------------------------------------------
;
; Mouse interrupt handler
;

IRQDone:rts

MouseIRQ:
	lda	Initialized		; Mouse initialized?
       	beq    	IRQDone			; Jump if no

	ldy	#SID_ADConv1
	lda	(sid),y	    		; Get mouse X movement
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

  	ldy     #SID_ADConv2
       	lda    	(sid),y	    		; Get mouse Y movement
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

; Move the mouse sprite to the current mouse position. Must be called
; with interrupts off and the system bank enabled. MoveSprite1 is an entry
; without checking.

MoveSprite:

 	lda   	Invisible		; Mouse visible?
       	bne    	Done			; Jump if no

; Set the high X bit

MoveSprite1:
	ldy     #VIC_SPR_HI_X
       	lda    	(vic),y	     		; Get high X bits of all sprites
   	and	#$FE			; Clear bit for sprite #0
       	ldx	XPos+1 			; Test Y position
   	beq	@L5
       	ora    	#$01		        ; Set high X bit
@L5:	sta    	(vic),y			; Set hi X sprite values

; Set the low X byte

   	lda	XPos
	ldy	#VIC_SPR0_X
	sta    	(vic),y	   		; Set low byte

; Set the Y position

       	ldy	YPos+1 	      		; Negative or too large?
       	bne    	Done 			; Jump if yes
   	lda	YPos
	ldy	#VIC_SPR0_Y
       	sta    	(vic),y	   		; Set Y position

; Done

Done:   rts

; --------------------------------------------------------------------------
;
; Move check routine, called for both coordinates.
;
; Entry:   	y = old value of pot register
;     	   	a = current value of pot register
; Exit:	   	y = value to use for old value
;     	   	x/a = delta value for position
;

.proc	MoveCheck

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

.endproc

; --------------------------------------------------------------------------
; Data

.bss

Initialized:   	.res   	1 		; True if mouse initialized
OldInitStatus:	.res	1		; Old IRQ flag value
OldValue:	.res   	1		; Temp for MoveCheck routine
NewValue:	.res   	1		; Temp for MoveCheck routine
YCorr:		.res	1		; Correction for Y coordinate

Invisible:	.res   	1		; Is the mouse invisible?
OldPotX:   	.res   	1		; Old hw counter values
OldPotY:	.res   	1

XPos:		.res   	2		; Current mouse position, X
YPos:		.res   	2		; Current mouse position, Y

XMin:		.res	2		; X1 value of bounding box
YMin:		.res	2		; Y1 value of bounding box
XMax:		.res	2		; X2 value of bounding box
YMax:		.res	2		; Y2 value of bounding box


