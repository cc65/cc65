;
; Standard joystick driver for the Creativision.
;
; Christian Groessler, 2017-02-06
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "creativision.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _creativisionstd_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $10                     ; JOY_UP
        .byte   $04                     ; JOY_DOWN
        .byte   $20                     ; JOY_LEFT
        .byte   $08                     ; JOY_RIGHT
        .byte   $01                     ; JOY_FIRE (button #1)
        .byte   $02                     ; JOY_FIRE2 (button #2)
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY
        .addr   0                       ; IRQ entry not used

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 2             ; Number of joysticks we support

; ------------------------------------------------------------------------
; Code

        .code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #JOY_ERR_OK
        ldx     #0
;       rts                     ; Fall through

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
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READJOY:
        and     #1              ; fix joystick number
        bne     READJOY_1       ; read right joystick

; Read left joystick

        ldx     ZP_JOY0_DIR
        lda     ZP_JOY0_BUTTONS
        jmp     convert         ; convert joystick state to sane cc65 values

; Read right joystick

READJOY_1:

        ldx     ZP_JOY1_DIR
        lda     ZP_JOY1_BUTTONS
        lsr     a
        lsr     a
        ;jmp    convert         ; convert joystick state to sane cc65 values
                                ; fall thru...

; ------------------------------------------------------------------------
; convert: make runtime lib compatible values
;       A - buttons
;       X - direction
;

convert:
        ldy     #0
        sty     retval          ; initialize return value

; ------
; buttons:
        ; Port values are for the left hand joystick (right hand joystick
        ; values were shifted to the right to be identical).
        ; Why are there two bits indicating a pressed trigger?
        ; According to the "Second book of programs for the Dick Smith Wizard"
	; (pg. 88ff), the left hand fire button gives the value of
        ; %00010001 and the right hand button gives %00100010
        ; Why two bits? Am I missing something? Can there be cases that just
        ; one of those bits is set?
        ; We just test if any of those two bits is not zero...

        tay
        and     #%00010001
        beq     cnv_1

        inc     retval           ; left button pressed

cnv_1:  tya
        and     #%00100010
        beq     cnv_2

        lda     #$02
        ora     retval
        sta     retval           ; right button pressed

; ------
; direction:
cnv_2:  txa
	; tested with https://sourceforge.net/projects/creativisionemulator
	; $49 - %01001001 - up
	; $41 - %01000001 - down
	; $4D - %01001101 - left
	; $45 - %01000101 - right
	;
	; are these correct? "Second book of programs for the Dick Smith Wizard" pg. 85 says something different
	; ignored for now...
	; $85 - %10000101 - up + right
	; $8D - %10001101 - down + left
	; $89 - %10001001 - up + left
	; $85 - %10000101 - down + right (emulator bug?)

	bit	testbit		; bit #0 set?
	beq	done		; no, no direction

	and	#%00001100	; mask out other bits
	tax
	lda	#%00000100	; init bitmask
loop:	dex
	bmi	done2
	asl	a
	bne	loop

done2:	ora	retval
	rts

done:	lda	retval
	rts

; ------------------------------------------------------------------------
;
        .data
testbit:.byte	$01

; ------------------------------------------------------------------------
;
        .bss
retval: .res    0
