;
; Joystick driver using C128 number pad in 64 mode.
; May be used multiple times when linked to the statically application.
;
; Stefan Haubenthal, 2004-01-26
; Based on Ullrich von Bassewitz, 2002-12-20
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "c64.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_numpad_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $02                     ; JOY_UP        "8"
        .byte   $10                     ; JOY_DOWN      "2"
        .byte   $20                     ; JOY_LEFT      "4"
        .byte   $08                     ; JOY_RIGHT     "6"
        .byte   $04                     ; JOY_FIRE      "5" ENTER
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

JOY_COUNT       = 1             ; Number of joysticks we support


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
        lda     #JOY_ERR_OK             ; Assume we have a joystick
        ldx     VIC_CLK_128             ; Test for a C128
        cpx     #$FF
        bne     @C128                   ; Jump if we have one
        lda     #JOY_ERR_NO_DEVICE      ; No C128 -> no numpad
@C128:  ldx     #0                      ; Set high byte

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

COUNT:  lda     #JOY_COUNT
        ldx     #0
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READ:   tax                     ; Clear high byte
        lda     #$FD
        ldy     #$FE
        sei
        sta     VIC_KBD_128
        lda     CIA1_PRB
        and     #%00110000
        eor     #%00110000
        lsr
        lsr
        sty     VIC_KBD_128
        eor     CIA1_PRB
        iny
        sty     VIC_KBD_128     ; Reset to $FF
        cli
        and     #%11111110
        eor     #%11111110
        rts

