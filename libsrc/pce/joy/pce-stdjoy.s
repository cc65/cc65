
;
; Standard joystick driver for the PCEngine
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "pce.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _pce_stdjoy_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $10                     ; JOY_UP
        .byte   $40                     ; JOY_DOWN
        .byte   $80                     ; JOY_LEFT
        .byte   $20                     ; JOY_RIGHT
        .byte   $01                     ; JOY_FIRE_A
        .byte   $02                     ; JOY_FIRE_B
        .byte   $04                     ; JOY_SELECT
        .byte   $08                     ; JOY_RUN

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY
        .addr   0                       ; IRQ entry unused

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support


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

;        rts                     ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; DEINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;
;unsigned char __fastcall__ joy_count (void);

COUNT:
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;
;unsigned char __fastcall__ joy_read (unsigned char joystick);

READJOY:
        pha
        jsr     read_joy
        pla
        tax                     ; Joystick number into X

        ; return value from buffer

joy1:
        lda     padbuffer,x
        ldx     #0
        rts

read_joy:
        ; reset multitap counter
        lda     #$01
        sta     JOY_CTRL
        pha
        pla
        nop
        nop

        lda     #$03
        sta     JOY_CTRL
        pha
        pla
        nop
        nop

        cly
nextpad:
        lda     #$01
        sta     JOY_CTRL           ; sel = 1
        pha
        pla
        nop                     ; some delay is required
        nop

        lda     JOY_CTRL
        asl     a
        asl     a
        asl     a
        asl     a
        sta     padbuffer, y     ; store new value

        stz     JOY_CTRL
        pha
        pla

        nop                     ; some delay is required
        nop

        lda     JOY_CTRL
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
        .res    4

