;
; DXS/HIT-4 Player joystick driver for the C64
;
; Groepaz/Hitmen, 2002-12-23
; obviously based on Ullrichs driver :)
;

        .include "zeropage.inc"

        .include "joy-kernel.inc"
        .include "joy-error.inc"
        .include "c64.inc"

        .macpack generic
        .macpack module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_hitjoy_joy

; Driver signature

        .byte   $6A, $6F, $79   ; "joy"
        .byte   JOY_API_VERSION ; Driver API version number

; Library reference

        .addr   $0000

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support

; ------------------------------------------------------------------------
; Data. Written in the IRQ, read by the READ routine

.bss

temp3:  .byte 0
temp4:  .byte 0

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #JOY_ERR_OK
        .assert JOY_ERR_OK = 0, error
        tax

;       rts             ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts

; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;

COUNT:  lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

readadapter:

        sei

        ; cia 2 setup
        ldy     #$00            ; port b direction
        sty     $dd03           ; => input

        sty     $dd05           ; cia2 timer a highbyte
        sty     $dc05           ; cia1 timer a highbyte
        iny
        sty     $dd04           ; cia2 timer a lowbyte
        sty     $dc04           ; cia1 timer a lowbyte

        lda     #%00010001
        sta     $dd0e           ; control register a
                                ; timer: start
                                ;        continous
                                ;        forced load
                                ; serial port: input

        ; cia 1 setup
        lda     #%01010001
        sta     $dc0e           ; control register a
                                ; timer: start
                                ;        continous
                                ;        forced load
                                ; serial port: output

        ; read directions 3
        lda     $dd01           ;read cia 2 port b
        and     #$0f
        sta     temp3

        ; read button 3
        lda     $dd02           ;cia 2 port a
        and     #%11111011      ;data direction
        sta     $dd02           ;=> bit 2 input

        lda     $dd00           ;read cia 2 p.A
        and     #%00000100      ;check bit 2
        asl     a
        asl     a
        ora     temp3
        sta     temp3

        ; read directions 4
        lda     $dd01           ;read cia 2 port b
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        sta     temp4

        ; read button 4
        ldx     #$ff            ;serial data register
        stx     $dc0c           ;=> writing $ff causes
                                ;cia to output some
                                ;count signals at cnt1

        ldx     $dd0c           ;read cia 2 serial in
        beq     fire            ;button press if zero

        lda     temp4
        ora     #%00010000
        sta     temp4

fire:
        ; FIXME: to be really 100% correct this should restore the correct timer
        ;        values for the respective machine (PAL: $4025, NTSC: $4295)
        ;        however, this should hardly be a problem in a real world program

        lda     #$41
        sta     $dc05
        lda     #0
        sta     $dc04

        cli
        rts

READ:
        pha
        jsr readadapter
        pla

        tax            ; Joystick number into X
        bne joy2

; Read joystick 1

joy1:   lda #$7F
        sei
        sta CIA1_PRA
        lda CIA1_PRB
        cli
        and #$1F
        eor #$1F
        rts

; Read joystick 2

joy2:   dex
        bne joy3

        ; ldx   #0
        lda     #$E0
        ldy     #$FF
        sei
        sta     CIA1_DDRA
        lda     CIA1_PRA
        sty     CIA1_DDRA
        cli
        and     #$1F
        eor     #$1F
        rts

        ; Read joystick 3

joy3:   dex
        bne     joy4

        lda     temp3
        eor     #$1F
        rts

        ; Read joystick 4

joy4:   lda     temp4
        eor     #$1F
        ldx     #0
        rts
