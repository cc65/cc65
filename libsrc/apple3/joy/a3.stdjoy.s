;
; Standard joystick driver for the Apple ///. May be used multiple times
; when statically linked to the application.
;
; Ullrich von Bassewitz, 2003-05-02
; Oliver Schmidt, 2008-02-25
; Robert Justice, 2026
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "apple3.inc"
        .include        "../sos.inc"
        
        .macpack        module

; ------------------------------------------------------------------------

; Constants

LOWER_THRESHOLD =   128-50
UPPER_THRESHOLD =   128+50

; ------------------------------------------------------------------------

; Header. Includes jump table.

        module_header   _a3_stdjoy_joy

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

        .data

; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
INSTALL:
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
        lda     #$02            ; Number of joysticks we support
        ldx     #>$0000
        rts

; READ routine. Read a particular joystick passed in A.
READ:
        beq     :+
        lda     #3              ; joystick B
        bne     :++
:       lda     #7              ; joystick A
:       sta     mode

        brk
        .byte   GET_ANALOG_CALL
        .addr   joylist

        ; Transform paddle readings to directions
        lda     #$00            ; 0 0 0 0 0 0 0 0
        ldy     status + 2
        cpy     #LOWER_THRESHOLD
        ror                     ; !LEFT 0 0 0 0 0 0 0
        cpy     #UPPER_THRESHOLD
        ror                     ; RIGHT !LEFT 0 0 0 0 0 0
        ldy     status + 3
        cpy     #LOWER_THRESHOLD
        ror                     ; !UP RIGHT !LEFT 0 0 0 0 0
        cpy     #UPPER_THRESHOLD
        ror                     ; DOWN !UP RIGHT !LEFT 0 0 0 0

        ; Read primary button
        tay
        lda     status
        asl
        tya
        ror                     ; BTN_1 DOWN !UP RIGHT !LEFT 0 0 0

        ; Read secondary button
        tay
        lda     status + 1
        asl
        tya
        ror                     ; BTN_2 BTN_1 DOWN !UP RIGHT !LEFT 0 0

        ; Finalize
        eor     #%00010100      ; BTN_2 BTN_1 DOWN UP RIGHT LEFT 0 0
        ldx     #>$0000
        rts

        .data

; get_analog param list
joylist:
        .byte   2
mode:   .byte   0
status: .byte   4
