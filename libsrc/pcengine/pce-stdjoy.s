
;
; Standard joystick driver for the PCEngine
;
; Ullrich von Bassewitz, 2002-12-20
;

		;;.include 	"zeropage.inc"

      	;;.include 	"joy-kernel.inc"
        
		;;.include    "joy-error.inc"
		JOY_ERR_OK=0;
        .include    "pcengine.inc"

        .macpack        generic

; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "CODE"

; Driver signature

;;        .byte   $6A, $6F, $79		; "joy"
;;        .byte   $00                     ; Driver API version number

; Button state masks (8 values)

;extern const unsigned char joy_masks[8];

		.export _joy_masks

_joy_masks:
        .byte   $10                     ; JOY_UP
        .byte   $40                     ; JOY_DOWN
        .byte   $80                     ; JOY_LEFT
        .byte   $20                     ; JOY_RIGHT
        .byte   $04                     ; ? JOY_FIRE
        .byte   $02                     ; ? Future expansion
        .byte   $01                     ; ? Future expansion
        .byte   $08                     ; ? Future expansion

; Jump table.

;;        .word   INSTALL
;;        .word   DEINSTALL
;;        .word   COUNT
;;        .word   READ

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support


; ------------------------------------------------------------------------
; Data.


.code


;extern const char joy_stddrv[];

		.export _joy_stddrv
_joy_stddrv:
		.byte 0


		.export _joy_load_driver
		.export _joy_unload

;unsigned char __fastcall__ joy_unload (void);
;unsigned char __fastcall__ joy_load_driver (const char* driver);
_joy_load_driver:
_joy_unload:

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
;unsigned char __fastcall__ joy_count (void);

		.export _joy_count

_joy_count:
COUNT:
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;
;unsigned char __fastcall__ joy_read (unsigned char joystick);

		.export _joy_read

_joy_read:
READ:
		pha
        jsr read_joy
        pla
		tax		; Joystick number into X

        ; return value from buffer

joy1:
        lda padbuffer,x
		ldx #0
     	rts

.code

read_joy:
        ; reset multitap counter
        lda     #$01
        sta     $1000
        pha
        pla
        nop
        nop

        lda     #$03
        sta     $1000
        pha
        pla
        nop
        nop

        cly
nextpad:
        lda     #$01
        sta     $1000   ; sel = 1
        pha
        pla
        nop
        nop

        lda     $1000
        asl     a
        asl     a
        asl     a
        asl     a
        sta     padbuffer, y     ; store new value

        stz     $1000
        pha
        pla
        nop
        nop

        lda     $1000
        and     #$0F
        ora     padbuffer, y     ; second half of new value

        eor     #$FF
        sta     padbuffer, y     ; store new value

        iny
        cpy     #$05
        bcc     nextpad
        rts

.bss

padbuffer:
        .res 4

