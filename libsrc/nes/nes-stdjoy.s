;
; Standard joypad driver for the NES. May be used multiple times when
; linked to the statically application.
;
; Ullrich von Bassewitz, 2003-05-02
; Stefan Haubenthal, 2004-10-05
;

	.include	"zeropage.inc"

	.include	"joy-kernel.inc"
	.include	"joy-error.inc"
	.include	"nes.inc"


; ------------------------------------------------------------------------
; Header. Includes jump table

.segment	"JUMPTABLE"

; Driver signature

	.byte	$6A, $6F, $79		; "joy"
	.byte	JOY_API_VERSION		; Driver API version number

; Button state masks (8 values)

	.byte	$08			; JOY_UP
	.byte	$04			; JOY_DOWN
	.byte	$02			; JOY_LEFT
	.byte	$01			; JOY_RIGHT
	.byte	$80			; JOY_FIRE  (A)
	.byte	$40			; JOY_FIRE2 (B)
	.byte	$20			; JOY_FIRE3 (Select)
	.byte	$10			; JOY_FIRE4 (Start)

; Jump table.

       	.addr   INSTALL
       	.addr   UNINSTALL
       	.addr   COUNT
       	.addr   READJOY
        .addr   0                       ; IRQ entry unused

; ------------------------------------------------------------------------
; Constants

JOY_COUNT	= 2		; Number of joysticks we support


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
	lda	#JOY_ERR_OK
	ldx	#0
;	rts			; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
	rts


; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;

COUNT:
	lda	#JOY_COUNT
	ldx	#0
	rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READJOY:
	and	#$01		; Fix joystick number
	tay			; Joystick number (0,1) into Y

; Read joystick

	lda	#1
	sta	APU_PAD1,y
	lda	#0
	sta	APU_PAD1,y
	sta	tmp1

	ldx	#8
@L1:	asl	tmp1
	lda	APU_PAD1,y
	and	#$01
	beq	@L2
	lda	tmp1
	ora	#$01
	sta	tmp1
@L2:	dex
	bne	@L1

	lda	tmp1
;	ldx	#$00		; fix X
	rts
