;
; Maciej 'YTM/Elysium' Witkowiak
;
; 2.7.2001
;
; Wrapper for GEOS standard input device interface
;

   	.export	    	_mouse_init, _mouse_done
   	.export	    	_mouse_hide, _mouse_show
   	.export	    	_mouse_box
   	.export		_mouse_pos, _mouse_info
   	.export	    	_mouse_move, _mouse_buttons

       	.import	       	popax, popsreg, addysp1
   	.importzp   	sp, sreg, ptr1

	.include "../inc/const.inc"
	.include "../inc/jumptab.inc"
	.include "../inc/geossym.inc"


.code

; --------------------------------------------------------------------------
;
; unsigned char __fastcall__ mouse_init (unsigned char port,
;					 unsigned char sprite,
;					 unsigned char type);
;

_mouse_init:
	jsr	popax			; ignore all parameters

	jsr	StartMouseMode
	jsr	MouseOff

	lda	#0
	sta	mouseTop
	sta	mouseLeft
	sta	mouseLeft+1
	lda	#199
	sta	mouseBottom
	lda	#<319
	sta	mouseRight
	lda	#>319
	sta	mouseRight+1

	lda 	#0
; --------------------------------------------------------------------------
;
; void mouse_done (void);
;
_mouse_done:
	rts

; --------------------------------------------------------------------------
;
; void mouse_hide (void);
;

_mouse_hide = MouseOff

; --------------------------------------------------------------------------
;
; void mouse_show (void);
;

_mouse_show = MouseUp

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
;

_mouse_box:
   	ldy 	#0		  	; Stack offset

   	sta 	mouseBottom

   	lda 	(sp),y
   	sta 	mouseRight
   	iny
   	lda 	(sp),y
   	sta 	mouseRight+1		; maxx

   	iny
   	lda	(sp),y
   	sta	mouseTop
   	iny				; Skip high byte

   	iny
   	lda	(sp),y
   	sta	mouseLeft
   	iny
   	lda	(sp),y
   	sta	mouseLeft+1		; minx

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

       	lda     mouseXPos		; Transfer the position
	sta	(ptr1),y
	lda	mouseXPos+1
	iny
	sta	(ptr1),y
      	lda	mouseYPos
      	iny
      	sta	(ptr1),y
      	lda	#$00
	iny
	sta	(ptr1),y

	plp				; Reenable interrupts
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
	iny
	sta	(ptr1),y

      	rts

; --------------------------------------------------------------------------
;
; void __fastcall__ mouse_move (int x, int y);
;

_mouse_move:
	jsr	popsreg			; Get X
        php
        sei				; Disable interrupts
	sta	mouseYPos
	lda	sreg
	ldx	sreg+1
   	sta	mouseXPos
	stx	mouseXPos+1
	plp		   		; Enable interrupts
	rts

; --------------------------------------------------------------------------
;
; unsigned char mouse_buttons (void);
;

_mouse_buttons:
	lda	pressFlag
	and	#SET_MOUSE
	lsr
	rts

