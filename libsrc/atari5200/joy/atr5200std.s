;
; Standard joystick driver for the Atari 5200.
;
; Christian Groessler, 2014-05-28
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atari5200.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _atr5200std_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $01             ; JOY_UP
        .byte   $02             ; JOY_DOWN
        .byte   $04             ; JOY_LEFT
        .byte   $08             ; JOY_RIGHT
        .byte   $10             ; JOY_FIRE
        .byte   $20             ; JOY_FIRE2
        .byte   $00             ; Future expansion
        .byte   $00             ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY
        .addr   0               ; IRQ entry not used

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
;       rts                     ; Run into UNINSTALL instead

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
        lda     $FD32           ; check ROM version
        cmp     #$E8
        bne     @2port
        lda     #4
        .byte   $2C             ; bit opcode, eats the next 2 bytes
@2port: lda     #2
        ldx     #0
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

CENTER  =       228 / 2
SENSIVITY       = 16

READJOY:
        and     #3              ; put joystick number in range, just in case
        tay
        asl     a
        tax                     ; Joystick number * 2 (0-6) into X, index into ZP shadow registers

        lda     #0              ; Initialize return value
        cmp     TRIG0,y
        bne     @notrg
        lda     #$10            ; JOY_FIRE

; Read joystick

@notrg: ldy     PADDL0,x        ; get horizontal position
        cpy     #CENTER-SENSIVITY
        bcs     @chkleft

        ora     #4              ; JOY_LEFT
        bne     @updown

@chkleft:
        cpy     #CENTER+SENSIVITY
        bcc     @updown

        ora     #8              ; JOY_RIGHT

@updown:ldy     PADDL0+1,x      ; get vertical position
        cpy     #CENTER-SENSIVITY
        bcs     @chkdown

        ora     #1              ; JOY_UP
        bne     @done

@chkdown:
        cpy     #CENTER+SENSIVITY
        bcc     @done

        ora     #2              ; JOY_DOWN

@done:  rts
