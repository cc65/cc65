;
; P/M mouse callbacks for the Ataris
;
; Christian Groessler, 07.04.2014
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .include        "atari.inc"
	.importzp	sp
        .constructor    pm_init,27
        .destructor     pm_down,7
        .export         _mouse_pm_callbacks


; P/M definitions. The first value can be changed to adjust the number
; of the P/M used for the mouse. All others depend on this value.
; Valid P/M numbers are 0 to 4. When 4 is used, the missiles are used
; as a player.
MOUSE_PM_NUM    = 4                             ; P/M used for the mouse
MOUSE_PM_BASE   = pm_base

.if MOUSE_PM_NUM = 4
MOUSE_PM_RAW	= 0
.macro	set_mouse_x
	; assume CF = 0
	sta	HPOSM3
	adc	#2
	sta	HPOSM2
	adc	#2
	sta	HPOSM1
	adc	#2
	sta	HPOSM0
.endmacro
.else
MOUSE_PM_RAW	= MOUSE_PM_NUM + 1
.macro	set_mouse_x
	sta     HPOSP0 + MOUSE_PM_NUM
.endmacro
.endif

; ------------------------------------------------------------------------

        .rodata

        ; Callback structure
_mouse_pm_callbacks:
        .addr   hide
        .addr   show
        .addr   prep
        .addr   draw
        .addr   movex
        .addr   movey

; ------------------------------------------------------------------------

        .bss

omy:    .res    1                       ; Old Mouse Y position

; ------------------------------------------------------------------------

        .segment "EXTZP" : zeropage

pm_base:.res    2

; ------------------------------------------------------------------------

        .code

; Hide the mouse cursor.
hide:   lda     #0
        sta     GRACTL
	rts

; Show the mouse cursor.
show:
.if MOUSE_PM_NUM < 4
        lda     #2
.else
        lda     #1
.endif
        sta     GRACTL
        ;rts

prep:
draw:
        rts

; Move the mouse cursor x position to the value in A/X.
movex:	cpx	#1
	ror	a
	clc
	adc	#48
	set_mouse_x
        rts

; Move the mouse cursor y position to the value in A/X.
movey:  clc
	adc	#32
	pha
        lda     omy
        jsr     clr_pm                  ; remove player at old position
        pla
        sta     omy
        ;jmp    set_pm                  ; put player to new position

; Set P/M data from 'mouse_bits'
set_pm: tay
        ldx     #0
set_l:  lda     mouse_bits,x
        sta     (MOUSE_PM_BASE),y
        inx
        iny
        cpx     #mouse_height
        bcc     set_l
        rts

; Clear (zero) P/M data
clr_pm: ldx     #mouse_height
        tay
        lda     #0
clr_l:  sta     (MOUSE_PM_BASE),y
        iny
        dex
        bne     clr_l
        rts


pm_down	= hide


; ------------------------------------------------------------------------

	.segment "INIT"

pm_init:lda	#0
	sta	sp
	sta	MOUSE_PM_BASE
	lda	sp+1
	and	#7			; offset within 2K
	cmp	#3 + MOUSE_PM_RAW + 1	; can we use it?
	bcc	@decr			; no

	lda	sp+1
	and	#$F8
@set:	adc	#3 + MOUSE_PM_RAW - 1	; CF is set, so adding MOUSE_PM_RAW + 3
	sta	MOUSE_PM_BASE+1
	sta	sp+1
	bne	@cont

@decr:	lda	sp+1
	and	#$F8
	sbc	#8 - 1			; CF is clear, subtracts 8
	bcs	@set			; jump always

@cont:	lda	#0
	tay
@iniloo:sta	(MOUSE_PM_BASE),y
	iny
	bne	@iniloo

	lda	MOUSE_PM_BASE+1
	and	#$F8
	sta	PMBASE

        lda     #62
        sta     SDMCTL

        lda     #1 + 16
        sta     GPRIOR

        lda     #0

.if MOUSE_PM_NUM = 4
	sta	PCOLR0
	sta	PCOLR1
	sta	PCOLR2
	sta	PCOLR3
	sta	SIZEM
.else
        sta     PCOLR0 + MOUSE_PM_NUM
        sta     SIZEP0 + MOUSE_PM_NUM
.endif
        rts


; ------------------------------------------------------------------------

        .data

mouse_bits:
        .byte   %11110000
        .byte   %11000000
        .byte   %10100000
        .byte   %10010000
        .byte   %10001000
        .byte   %00000100
        .byte   %00000010
;       .byte   %00000000

mouse_height = * - mouse_bits

