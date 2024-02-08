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

LOWER_THRESHOLD =   05
UPPER_THRESHOLD =   85

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

libref: .addr   $0000

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ

; ------------------------------------------------------------------------

        .bss

ostype: .res    1
value0: .res    1
value1: .res    1

; ------------------------------------------------------------------------

        .data

; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
INSTALL:
        lda     libref
        ldx     libref+1
        sta     gettype+1
        stx     gettype+2
gettype:jsr     $0000
        sta     ostype
        lda     #JOY_ERR_OK
        .assert JOY_ERR_OK = 0, error
        tax
        ; Fall through

; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
UNINSTALL:
        rts

; ------------------------------------------------------------------------

        .code

; COUNT routine. Return the total number of available joysticks in a/x.
COUNT:
        ldx     #$02
        bit     ostype
        bvc     noiic           ; Not $4x
        dex                     ; Only one joystick for the //c
noiic:  txa                     ; Number of joysticks we support
        ldx     #$00
        rts

; READ routine. Read a particular joystick passed in A.
READ:
        asl                     ; Joystick number -> paddle number
        tax
        ldy     #$00
        sty     value0
        sty     value1

        ; If IIgs -> set speed to normal
        bit     ostype
        bpl     nogs1           ; Not $8x
        lda     CYAREG
        pha
        and     #%01111111
        sta     CYAREG

        ; Read both paddles simultaneously according to:
        ; Apple IIe Technote #6, The Apple II Paddle Circuits
nogs1:  lda     PTRIG           ; Trigger paddles
loop:   lda     PADDL0,x        ; Read paddle (0 or 2)
        bmi     set0            ; Cycles:   2   3
        nop                     ; Cycles:   2
        bpl     nop0            ; Cycles:   3
set0:   sty     value0          ; Cycles:       4
nop0:                           ;           -   -
                                ; Cycles:   7   7
        lda     PADDL1,x        ; Read paddle (1 or 3)
        bmi     set1            ; Cycles:   2   3
        nop                     ; Cycles:   2
        bpl     nop1            ; Cycles:   3
set1:   sty     value1          ; Cycles:       4
nop1:                           ;           -   -
                                ; Cycles:   7   7
        iny
        cpy     #UPPER_THRESHOLD+1
        bne     loop

        ; If IIgs -> restore speed
        bit     ostype
        bpl     nogs2           ; Not $8x
        pla
        sta     CYAREG

        ; Transform paddle readings to directions
nogs2:  lda     #$00            ; 0 0 0 0 0 0 0 0
        ldy     value0
        cpy     #LOWER_THRESHOLD
        ror                     ; !LEFT 0 0 0 0 0 0 0
        cpy     #UPPER_THRESHOLD
        ror                     ; RIGHT !LEFT 0 0 0 0 0 0
        ldy     value1
        cpy     #LOWER_THRESHOLD
        ror                     ; !UP RIGHT !LEFT 0 0 0 0 0
        cpy     #UPPER_THRESHOLD
        ror                     ; DOWN !UP RIGHT !LEFT 0 0 0 0

        ; Read primary button
        tay
        lda     BUTN0,x         ; Check button (0 or 2)
        asl
        tya
        ror                     ; BTN_1 DOWN !UP RIGHT !LEFT 0 0 0

        ; Read secondary button
        tay
        txa
        eor     #$02            ; IIgs has fourth button at TAPEIN
        tax
        lda     TAPEIN,x        ; Check button (1 or 3)
        asl
        tya
        ror                     ; BTN_2 BTN_1 DOWN !UP RIGHT !LEFT 0 0

        ; Finalize
        eor     #%00010100      ; BTN_2 BTN_1 DOWN UP RIGHT LEFT 0 0
        ldx     #$00
        rts
