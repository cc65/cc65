;
; Standard joystick driver for the Apple ][. May be used multiple times
; when statically linked to the application.
;
; Ullrich von Bassewitz, 2003-05-02
; Oliver Schmidt, 2008-02-25
; Using the readjoy code from Stefan Haubenthal
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "apple2.inc"

        .macpack        module

; ------------------------------------------------------------------------

; Constants

THRESHOLD =     20      ; Deviation from center triggering movement

; ------------------------------------------------------------------------

; ROM entry points

PREAD   :=      $FB1E   ; Read paddle in X, return AD conv. value in Y

; ------------------------------------------------------------------------

; Header. Includes jump table.

        .ifdef  __APPLE2ENH__
        module_header   _a2e_stdjoy_joy
        .else
        module_header   _a2_stdjoy_joy
        .endif

; Driver signature

        .byte   $6A, $6F, $79   ; "joy"
        .byte   JOY_API_VERSION ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $10
        .byte   $20
        .byte   $04
        .byte   $08
        .byte   $40
        .byte   $80
        .byte   $00             ; Future expansion
        .byte   $00             ; Future expansion

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY
        .addr   0               ; IRQ not used

; ------------------------------------------------------------------------

        .code

; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
INSTALL:
        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK
        ; Fall through

; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
UNINSTALL:
        rts

; COUNT: Return the total number of available joysticks in a/x.
COUNT:
        lda     #$02            ; Number of joysticks we support
        ldx     #$00
        rts

; READ: Read a particular joystick passed in A.
READJOY:
        bit     $C082           ; Switch in ROM
        and     #$01            ; Restrict joystick number

        ; Read horizontal paddle
        asl                     ; Joystick number -> paddle number
        tax                     ; Set paddle number (0, 2)
        jsr     PREAD           ; Read paddle value
        lda     #$00            ; 0 0 0 0 0 0 0 0
        cpy     #127 - THRESHOLD
        ror                     ; !LEFT 0 0 0 0 0 0 0
        cpy     #127 + THRESHOLD
        ror                     ; RIGHT !LEFT 0 0 0 0 0 0

        ; Read vertical paddle
        pha
        inx                     ; Set paddle number (1, 3)
        jsr     PREAD           ; Read paddle value
        pla
        cpy     #127 - THRESHOLD
        ror                     ; !UP RIGHT !LEFT 0 0 0 0 0
        cpy     #127 + THRESHOLD
        ror                     ; DOWN !UP RIGHT !LEFT 0 0 0 0

        ; Read primary button
        tay
        lda     BUTN0-1,x       ; Check button (1, 3)
        asl
        tya
        ror                     ; FIRE DOWN !UP RIGHT !LEFT 0 0 0

        ; Read secondary button
        tay
        inx
        txa
        and     #$03            ; IIgs has fourth button at TAPEIN
        tax
        lda     BUTN0-1,x       ; Check button (2, 0)
        asl
        tya
        ror                     ; FIRE2 FIRE DOWN !UP RIGHT !LEFT 0 0

        ; Finalize
        eor     #%00010100      ; FIRE2 FIRE DOWN UP RIGHT LEFT 0 0
        ldx     #$00
        bit     $C080           ; Switch in LC bank 2 for R/O
        rts
