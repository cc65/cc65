;
; Standard joystick driver for the Apple ][. May be used multiple times when
; linked to the statically application.
;
; Ullrich von Bassewitz, 2003-05-02
; Using the readjoy code from Stefan Haubenthal
;

 	.include 	"zeropage.inc"

      	.include 	"joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "apple2.inc"

        .macpack        generic


; ------------------------------------------------------------------------
; Constants

OFFS            = 10

; ------------------------------------------------------------------------
; ROM entry points

PREAD  	:=  	$FB1E	; Read paddle in X, return AD conv. value in Y

; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

; Driver signature

        .byte   $6A, $6F, $79		; "joy"
        .byte   JOY_API_VERSION		; Driver API version number

; Button state masks (8 values)

        .byte   $40
        .byte   $80
        .byte   $10
        .byte   $20
        .byte   $08
        .byte   $00                     ; FIRE2 not available
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY
        .addr   0                       ; IRQ not used

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 2             ; Number of joysticks we support


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
;	rts                     ; Run into UNINSTALL instead

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
	and    	#$01            ; Fix joystick number
        asl     a               ;
	tax	     		; Joystick number (0,2) into X

; Read joystick

        lda     BUTN0,x		; Check fire button
        and     #$80            ; BTN 0 0 0 0 0 0 0

        pha
        jsr     PREAD           ; Read first paddle value
        pla
        cpy     #127-OFFS
        ror     a               ; /LEFT BTN 0 0 0 0 0 0
        cpy     #127+OFFS
        ror     a               ; RIGHT /LEFT BTN 0 0 0 0 0

        inx
        pha
        jsr     PREAD           ; Read second paddle
        pla
        cpy     #127-OFFS
        ror     a               ; /UP RIGHT /LEFT BTN 0 0 0 0
        cpy     #127+OFFS
        ror     a               ; DOWN /UP RIGHT /LEFT BTN 0 0 0
        eor     #%01010000      ; DOWN UP RIGHT LEFT BTN 0 0 0

        ldx     #$00		; fix X
        rts


