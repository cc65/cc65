;--------------------------------------------------------------------
; Atari 8-bit mouse routines -- 05/07/2000 Freddy Offenga
;
; The following devices are supported:
; - Atari trak-ball
; - ST mouse
; - Amiga mouse
;
; Mouse checks are done in the timer 1 IRQ and the mouse arrow is
; drawn in player 0 during the vertical blank
;--------------------------------------------------------------------

	.export	_mouse_init, _mouse_done, _mouse_box
	.export _mouse_show, _mouse_hide, _mouse_move
	.export _mouse_down

	.import popa

	.include "atari.inc"

TRAK_BALL	= 0	; device Atari trak-ball
ST_MOUSE	= 1	; device ST mouse
AMIGA_MOUSE	= 2	; device Amiga mouse
MAX_TYPE	= 3	; first illegal device type

defxmin = 48            ; default x minimum
defymin = 32            ; default y minimum

defxmax = 204           ; default x maximum
defymax = 211           ; default y maximum

pmsize  = 16            ; size pm shape

xinit   = 100            ; init. x pos.
yinit   = 100            ; init. y pos.

pmb	= $2800 	; pm base memory
pm0	= pmb+$400	; pm 0 memory

;--------------------------------------------------------------------
; Initialize mouse routines
; int mouse_init(unsigned char type, unsigned char port)

_mouse_init:
	ldy	#0
	sta	(SAVMSC),y
	sta	port_nr
	jsr	popa
	ldy	#1
	sta	(SAVMSC),y
	sta	mouse_type

	ldx	mouse_type
	cpx	#MAX_TYPE
	bcc	setup

ifail:
	lda	#0		; init. failed
	tax
	rts

setup:
	lda	lvectab,x
	sta	mouse_vec+1
	lda	hvectab,x
	sta	mouse_vec+2

        lda     #defxmin
        sta     xmin
        lda     #defymin
        sta     ymin

	lda	#defxmax
	sta	xmax
	lda	#defymax
	sta	ymax

	lda	#xinit
	sta	mousex
	lda	#yinit
	sta	mousey

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
	sta	mouse_on
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
; void mouse_box(char xmin, char ymin, char xmax, char ymax)

_mouse_box:
	sta	ymax
        jsr     popa
	sta	xmax
	jsr	popa
	sta	ymin
        jsr     popa
	sta	xmin
	rts

;--------------------------------------------------------------------
; Set mouse position
; void mouse_move(char xpos, char ypos)

_mouse_move:
	sta	mousey
	jsr	popa
	sta	mousex
	rts

;--------------------------------------------------------------------
; Show mouse arrow
; void mouse_show(void)

_mouse_show:
	lda	#1
	sta	mouse_on
	rts

;--------------------------------------------------------------------
; Hide mouse arrow
; void mouse_hide(void)

_mouse_hide:
	lda	#0
	sta	mouse_on
	rts

;--------------------------------------------------------------------
; Ask mouse button
; int mouse_down(void)

_mouse_down:
	ldx	port_nr
	lda	STRIG0,x
	bne	nobut
	lda	#14
	sta	COLOR1
	ldx	#0
	lda	#1
	rts
nobut:	ldx	#0
	txa
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

	lda	RANDOM
	sta	COLBK		; debug

	lda	port_nr
	lsr			; even number 0/2
	tay
	lda	PORTA,y
	ldy	port_nr
	cpy	#1
	beq	oddp
	cpy	#3
	beq	oddp

        lsr
	lsr
	lsr
	lsr
oddp:	tay

mouse_vec:
        jsr     st_check        ; will be modified

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
        jmp     SYSVBV          ; will be modified

;--------------------------------------------------------------------
; initialize mouse pm

pminit: ldx	#0
	txa
clpm:	sta	pm0,x
	inx
	bne	clpm

	lda	#>pmb
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
	sta	pm0,x
	inx
	iny
	cpy	#pmsize
	bne	fmp2
	rts

;--------------------------------------------------------------------
; clear old mouse pm

clrpm:	lda omy
        tax

	ldy #0
	tya
fmp1:	sta pm0,x
	inx
	iny
	cpy #pmsize
	bne fmp1
	rts

;--------------------------------------------------------------------

        .data

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

; Misc. vars

old_t1: .res 2

dumx:	.res 1
dumy:	.res 1
oldval: .res 1

omy:	.res 1

mousex: .res 1
mousey: .res 1

xmin:   .res 1
ymin:   .res 1

xmax:	.res 1
ymax:	.res 1

mouse_on:
	.res 1

mouse_type:
	.res 1

port_nr:
	.res 1
