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

JOY_UP          = $01
JOY_DOWN        = $02
JOY_LEFT        = $04
JOY_RIGHT       = $08
JOY_FIRE        = $10

OFFS            = 10

; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

; Driver signature

        .byte   $6A, $6F, $79		; "joy"
        .byte   $00                     ; Driver API version number

; Button state masks (8 values)

        .byte   JOY_UP
        .byte   JOY_DOWN
        .byte   JOY_LEFT
        .byte   JOY_RIGHT
        .byte   JOY_FIRE
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .word   INSTALL
        .word   UNINSTALL
        .word   COUNT
        .word   READJOY

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

;	rts                     ; Run into DEINSTALL instead

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

        lda     #$00            ; Clear result
        ldy     OPEN_APPLE,x    ; Check fire button
        bpl     @nofire
        ora     #JOY_FIRE       ; Fire button pressed

@nofire:
        pha
        jsr     PREAD           ; Read first paddle value
        pla
        cpy     #127-OFFS
        bcc     @left
        cpy     #127+OFFS
        bcc     @nextpaddle
        ora     #JOY_RIGHT
        .byte   $2c
@left:  ora     #JOY_LEFT

@nextpaddle:
        inx
        pha
        jsr     PREAD
        pla
        cpy     #127-OFFS
        bcc     @up
        cpy     #127+OFFS
        bcc     @done
        ora     #JOY_DOWN
        .byte   $2c
@up:    ora     #JOY_UP

@done:  ldx     #0              ; fix X
        rts


                                               
