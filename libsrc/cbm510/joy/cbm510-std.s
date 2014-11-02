;
; Standard joystick driver for the Commodore 510 (aka P500). May be used
; multiple times when linked to the statically application.
;
; Ullrich von Bassewitz, 2003-02-16
;

        .include        "zeropage.inc"
        .include        "../extzp.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "cbm510.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cbm510_std_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $01                     ; JOY_UP
        .byte   $02                     ; JOY_DOWN
        .byte   $04                     ; JOY_LEFT
        .byte   $08                     ; JOY_RIGHT
        .byte   $10                     ; JOY_FIRE
        .byte   $00                     ; JOY_FIRE2 unavailable
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ
        .addr   0                       ; IRQ entry unused

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
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READ:   ldx     #$0F            ; Switch to the system bank
        stx     IndReg
        tax                     ; Save joystick number

; Get the direction bits

        ldy     #CIA::PRB
        lda     (cia2),y        ; Read joystick inputs
        sta     tmp1

; Get the fire bits

        ldy     #CIA::PRA
        lda     (cia2),y

; Make the result value

        cpx     #$00            ; Joystick 0?
        bne     @L1             ; Jump if no

; Joystick 1, fire is in bit 6, direction in bit 0-3

        asl     a
        jmp     @L2

; Joystick 2, fire is in bit 7, direction in bit 5-7

@L1:    ldx     #$00            ; High byte of return value
        lsr     tmp1
        lsr     tmp1
        lsr     tmp1
        lsr     tmp1

; Mask the relavant bits, get the fire bit

@L2:    asl     a               ; Fire bit into carry
        lda     tmp1
        and     #$0F
        bcc     @L3
        ora     #$10
@L3:    eor     #$1F            ; All bits are inverted

; Switch back to the execution bank and return the joystick mask in a/x

        ldy     ExecReg
        sty     IndReg
        rts

