;
; Maciej 'YTM/Elysium' Witkowiak
;
; 2.7.2001
;
; Wrapper for GEOS standard input device interface
;

   	.export	    	_mouse_init, _mouse_done
   	.export	    	_mouse_hide, _mouse_show
   	.export	    	_mouse_box, _mouse_info
   	.export		_mouse_x, _mouse_y
   	.export	    	_mouse_move, _mouse_buttons
						   
       	.import	       	popax, popsreg, addysp1
   	.importzp   	sp, sreg

	.include "../inc/const.inc"
	.include "../inc/jumptab.inc"
	.include "../inc/geossym.inc"

;	.macpack	generic

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
   	ldy 	#0			; Stack offset

   	sta 	mouseBottom
;   	stx 	YMax+1			; maxy

   	lda 	(sp),y
   	sta 	mouseRight
   	iny
   	lda 	(sp),y
   	sta 	mouseRight+1		; maxx

   	iny
   	lda	(sp),y
   	sta	mouseTop
   	iny
;   	lda	(sp),y
;  	sta	YMin+1			; miny

   	iny
   	lda	(sp),y
   	sta	mouseLeft
   	iny
   	lda	(sp),y
   	sta	mouseLeft+1		; minx

   	jmp	addysp1			; Drop params, return

; --------------------------------------------------------------------------
;
; int __fastcall__ mouse_x (void);
;

_mouse_x:
	lda	mouseXPos
	ldx	mouseXPos+1
      	rts

; --------------------------------------------------------------------------
;
; int __fastcall__ mouse_y (void);
;

_mouse_y:
	lda	mouseYPos
	ldx	#0
      	rts

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

	sta	mouseYPos
;	stx	YPos+1
	lda	sreg
	ldx	sreg+1
   	sta	mouseXPos
	stx	mouseXPos+1
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
