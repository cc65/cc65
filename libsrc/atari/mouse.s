;--------------------------------------------------------------------
; Atari 8-bit mouse routines -- 05/07/2000 Freddy Offenga
; Some changes by Christian Groessler, Ullrich von Bassewitz
;
; The following devices are supported:
; - Atari trak-ball
; - ST mouse
; - Amiga mouse
;
; Mouse checks are done in the timer 1 IRQ and the mouse arrow is
; drawn in player 0 during the vertical blank
;--------------------------------------------------------------------

	.export		_mouse_init, _mouse_done, _mouse_box
	.export 	_mouse_show, _mouse_hide, _mouse_move
	.export 	_mouse_buttons, _mouse_pos, _mouse_info
	.constructor	initmouse,27

	.import 	popa,popax
	.importzp	ptr1

	.include "atari.inc"

TRAK_BALL	= 0	; device Atari trak-ball
ST_MOUSE	= 1	; device ST mouse
AMIGA_MOUSE	= 2	; device Amiga mouse
MAX_TYPE	= 3	; first illegal device type

; the default values force the mouse cursor inside the test screen (no access to border)
defxmin = 48		; default x minimum
defymin = 32		; default y minimum
defxmax = 204		; default x maximum
defymax = 211		; default y maximum

pmsize	= 16		; y size pm shape

xinit	= defxmin	; init. x pos.
yinit	= defymin	; init. y pos.

;--------------------------------------------------------------------
; reserve memory for the mouse pointer

initmouse:
	lda	APPMHI+1
	and	#%11111000	; make 2k aligned
	sec
	sbc	#%00001000	; reserve 2k
	tax
	adc	#3		; add 4 (C = 1)
	sta	mouse_pm0
	lda	#0
	sta	APPMHI
	stx	APPMHI+1
	rts


;--------------------------------------------------------------------
; Initialize mouse routines
; void __fastcall__ mouse_init (unsigned char port, unsigned char sprite, unsigned char type);

_mouse_init:
	pha			; remember mouse type
	jsr	popa		; ignore sprite / pm for now
	jsr	popa
	sta	port_nr
	pla			; get mouse type again

	cmp	#MAX_TYPE+1
	bcc	setup

ifail:	lda	#0		; init. failed
	tax
	rts

setup:	tax
	lda	lvectab,x
	sta	mouse_vec+1
	lda	hvectab,x
	sta	mouse_vec+2

	jsr	pminit

	lda	VTIMR1
	sta	old_t1
	lda	VTIMR1+1
	sta	old_t1+1

	lda	#<t1_vec
	sta	VTIMR1
	lda	#>t1_vec
	sta	VTIMR1+1

	lda	#%00000001
	sta	AUDCTL

	lda	#0
	sta	AUDC1

	lda	#15
	sta	AUDF1
	sta	STIMER

	sei
	lda	POKMSK
	ora	#%00000001		; timer 1 enable
	sta	POKMSK
	sta	IRQEN
	cli

	lda	VVBLKI
        sta     vbi_jmp+1
	lda	VVBLKI+1
        sta     vbi_jmp+2

	lda	#6
	ldy	#<vbi
	ldx	#>vbi
	jsr	SETVBV

	lda	#$C0
	sta	NMIEN

	ldx	#0
	lda	#1
	stx	mouse_on
	rts

;--------------------------------------------------------------------
; Finish mouse routines
; void mouse_done(void)

_mouse_done:
	sei
	lda	POKMSK
	and	#%11111110		; timer 1 disable
	sta	IRQEN
	sta	POKMSK
	cli

	lda	old_t1
	sta	VTIMR1
	lda	old_t1+1
	sta	VTIMR1+1

	lda	#$40
	sta	NMIEN

	lda	#6
        ldy     vbi_jmp+1
        ldx     vbi_jmp+2
	jsr	SETVBV

	lda	#0
	sta	GRACTL
	sta	HPOSP0
	sta	mouse_on
	rts

;--------------------------------------------------------------------
; Set mouse limits
; void __fastcall__ mouse_box(int xmin, int ymin, int xmax, int ymax)

_mouse_box:
	sta	ymax
        jsr     popax		; always ignore high byte
	sta	xmax
	jsr	popax
	sta	ymin
        jsr     popax
	sta	xmin
	rts

;--------------------------------------------------------------------
; Set mouse position
; void __fastcall__ mouse_move(int xpos, int ypos)

_mouse_move:
	sta	mousey		; always ignore high byte
	jsr	popax
	sta	mousex
	rts

;--------------------------------------------------------------------
; Show mouse arrow
; void mouse_show(void)

_mouse_show:
	inc	mouse_on
	rts

;--------------------------------------------------------------------
; Hide mouse arrow
; void mouse_hide(void)

_mouse_hide:
	lda     mouse_on
	beq	@L1
	dec	mouse_on
@L1:	rts

;--------------------------------------------------------------------
; Ask mouse button
; unsigned char mouse_buttons(void)

_mouse_buttons:
    	ldx	port_nr
    	lda	STRIG0,x
    	bne	nobut
;   	lda	#14
;???	sta	COLOR1
    	ldx	#0
    	lda	#1
    	rts
nobut:	ldx	#0
    	txa
    	rts

;--------------------------------------------------------------------
; Get the mouse position
; void mouse_pos (struct mouse_pos* pos);

_mouse_pos:
	sta	ptr1
	stx	ptr1+1			; Store argument pointer
	ldy	#0
	lda	mousex			; X position
	sta	(ptr1),y
	lda	#0
	iny
	sta	(ptr1),y
	lda	mousey			; Y position
	iny
	sta	(ptr1),y
	lda	#0
	iny
	sta	(ptr1),y
    	rts

;--------------------------------------------------------------------
; Get the mouse position and button information
; void mouse_info (struct mouse_info* info);

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

;--------------------------------------------------------------------
; Atari trak-ball check, A,Y = 4-bit port value

trak_check:
    	eor	oldval
    	and	#%00001000
    	beq	horiz

    	tya
    	and	#%00000100
	beq	mmup

	inc	mousey
	bne	horiz

mmup:	dec	mousey

horiz:	tya
	eor	oldval
	and	#%00000010
	beq	mmexit

	tya
	and	#%00000001
	beq	mmleft

	inc	mousex
	bne	mmexit

mmleft: dec	mousex

mmexit: sty	oldval
	rts

;--------------------------------------------------------------------
; ST mouse check, A,Y = 4-bit port value

st_check:
	and	#%00000011
	ora	dumx
	tax
	lda	sttab,x
	bmi	nxst

	beq	xist
	dec	mousex		    ; 1 = left
	bne	nxst
xist:	inc	mousex		    ; 0 = right

nxst:	tya
	and	#%00001100
	ora	dumy
	tax
	lda	sttab,x
	bmi	nyst

	bne	yst
	dec	mousey		    ; 0 = up
	bne	nyst
yst:	inc	mousey		    ; 1 = down

; store old readings

nyst:	tya
	and	#%00000011
	asl
	asl
	sta	dumx
	tya
	and	#%00001100
	lsr
	lsr
	sta	dumy
	rts

;--------------------------------------------------------------------
; Amiga mouse check, A,Y = 4-bit port value

amiga_check:

	lsr
	and	#%00000101
	ora	dumx
	tax
	lda	amitab,x
	bmi	nxami

	bne	xiami
	dec	mousex		    ; 0 = left
	bne	nxami
xiami:	inc	mousex		    ; 1 = right

nxami:	tya

	and	#%00000101
	ora	dumy
	tax
	lda	amitab,x
	bmi	nyami

	bne	yiami
	dec	mousey		    ; 0 =  up
	bne	nyami
yiami:	inc	mousey		    ; 1 = down

; store old readings

nyami:	tya
	and	#%00001010
	sta	dumx
	tya
	and	#%00000101
	asl
	sta	dumy
	rts

;--------------------------------------------------------------------
; timer 1 IRQ routine - check mouse

t1_vec: tya
	pha
	txa
	pha

.ifdef DEBUG
	lda	RANDOM
	sta	COLBK		; debug
.endif

	lda	port_nr
	lsr			; even number 0/2
	tay
	lda	PORTA,y
	ldy	port_nr
	cpy	#0
	beq	oddp
	cpy	#2
	beq	oddp

        lsr
	lsr
	lsr
	lsr
oddp:	tay

mouse_vec:
        jsr     st_check        ; will be modified; won't be ROMmable

	pla
	tax
	pla
	tay
        pla
	rti

;--------------------------------------------------------------------
; VBI - check mouse limits and display mouse arrow

vbi:	lda	mousex
	cmp	xmin
        bcs     ok1             ; xmin <= mousex
        lda     xmin
	sta	mousex

ok1:	lda	mousey
	cmp	ymin
        bcs     ok2             ; ymin <= mousey
	lda	ymin
	sta	mousey

ok2:    lda     xmax
        cmp     mousex
        bcs     ok3             ; xmax >= mousex
	lda	xmax
	sta	mousex

ok3:    lda     ymax
        cmp     mousey
        bcs     ok4             ; ymax >= mousey
	lda	ymax
	sta	mousey

ok4:    jsr     clrpm

	lda	mouse_on
        bne     mon
        lda     #0
        sta     HPOSP0
        beq     moff

mon:    jsr     drwpm
	lda	mousey
	sta	omy

        lda     #3
moff:	sta	GRACTL

vbi_jmp:
        jmp     SYSVBV          ; will be modified; won't be ROMmable

;--------------------------------------------------------------------
; initialize mouse pm

pminit:	lda	mouse_pm0
	sta	mpatch1+2
	sta	mpatch2+2
	sta	mpatch3+2

	ldx	#0
	txa
mpatch1:
clpm:	sta	$1000,x		; will be patched
	inx
	bne	clpm

	lda	mouse_pm0
	sec
	sbc	#4
	sta	PMBASE

	lda	#62
	sta	SDMCTL

        lda     #1
	sta	GPRIOR

	lda	#0
	sta	PCOLR0
        sta     SIZEP0
	rts

;--------------------------------------------------------------------
; draw new mouse pm

drwpm:	lda	mousex
	sta	HPOSP0

	lda	mousey
	tax

	ldy	#0
fmp2:	lda	mskpm,y
mpatch2:
	sta	$1000,x		; will be patched
	inx
	iny
	cpy	#pmsize
	bne	fmp2
	rts

;--------------------------------------------------------------------
; clear old mouse pm

clrpm:	lda	omy
        tax

	ldy	#0
	tya
mpatch3:
fmp1:	sta	$1000,x		; will be patched
	inx
	iny
	cpy	#pmsize
	bne	fmp1
	rts

;--------------------------------------------------------------------
        .rodata

; mouse arrow - pm shape

mskpm:	.byte %00000000
	.byte %10000000
	.byte %11000000
	.byte %11000000

	.byte %11100000
	.byte %11100000
	.byte %11110000
	.byte %11100000

	.byte %11100000
	.byte %00100000
	.byte %00100000
	.byte %00110000

	.byte %00110000
	.byte %00000000
	.byte %00000000
	.byte %00000000

; ST mouse lookup table

sttab:  .byte $FF,$01,$00,$01
        .byte $00,$FF,$00,$01
        .byte $01,$00,$FF,$00
        .byte $01,$00,$01,$FF

; Amiga mouse lookup table

amitab: .byte $FF,$01,$00,$FF
        .byte $00,$FF,$FF,$01
        .byte $01,$FF,$FF,$00
        .byte $FF,$00,$01,$FF

; Device vectors

lvectab:
	.byte <trak_check, <st_check, <amiga_check
hvectab:
	.byte >trak_check, >st_check, >amiga_check

; default values

xmin:   .byte	defxmin
ymin:   .byte	defymin
xmax:	.byte	defxmax
ymax:	.byte	defymax

mousex:	.byte	xinit
mousey:	.byte	yinit

;--------------------------------------------------------------------
	.bss

; Misc. vars

old_t1: .res 2		; old timer interrupt vector
oldval: .res 1		; used by trakball routines
dumx:	.res 1
dumy:	.res 1
omy:	.res 1		; old y pos

mouse_on:
	.res 1
port_nr:
	.res 1
mouse_pm0:
	.res 1
