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

        .byte   $6A, $6F, $79   ; "joy"
        .byte   JOY_API_VERSION ; Driver API version number

; Library reference

        .addr   $0000

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support


.bss

padbuffer:      .res    JOY_COUNT

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return a JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #JOY_ERR_OK
        .assert JOY_ERR_OK = 0, error
        tax

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
        clx                     ; ldx #>JOY_COUNT
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
        rts

read_joy:
        ; Reset Multitap counter.
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
        sta     JOY_CTRL        ; sel = 1
        pha
        pla
        nop                     ; some delay is required
        nop

        lda     JOY_CTRL
        asl     a
        asl     a
        asl     a
        asl     a
        sta     padbuffer,y     ; store new value

        stz     JOY_CTRL
        pha
        pla
        nop                     ; some delay is required
        nop

        lda     JOY_CTRL
        and     #$0F
        ora     padbuffer,y     ; second half of new value

        eor     #$FF
        sta     padbuffer,y     ; store new value

        iny
        cpy     #.sizeof(padbuffer)
        bcc     nextpad
        rts
