;
; Standard joystick driver for the Atari
;
; Ullrich von Bassewitz, 2002-12-21
; Using the readjoy code from Christian Groessler
;

 	.include 	"zeropage.inc"

      	.include 	"joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atari.inc"

        .macpack        generic


; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

; Driver signature

        .byte   $6A, $6F, $79		; "joy"
        .byte   $00                     ; Driver API version number

; Button state masks (8 values)

        .byte   $01                     ; JOY_UP
        .byte   $02                     ; JOY_DOWN
        .byte   $04                     ; JOY_LEFT
        .byte   $08                     ; JOY_RIGHT
        .byte   $10                     ; JOY_FIRE
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .word   INSTALL
        .word   DEINSTALL
        .word   COUNT
        .word   READJOY

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support


; ------------------------------------------------------------------------
; Data.


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK

;	rts                     ; Run into DEINSTALL instead

; ------------------------------------------------------------------------
; DEINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

DEINSTALL:
        rts


; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;

COUNT:
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READJOY:
	and	#3		; fix joystick number
	tax			; Joystick number (0-3) into X

; Read joystick

	lda	STRIG0,x	; get button
	asl	a
	asl	a
	asl	a
	asl	a
	ora	STICK0,x	; add position information
	eor	#$1F
	ldx	#0		; fix X
     	rts

