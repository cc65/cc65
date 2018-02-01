;
; Standard joystick driver for the Creativision.
;
; Christian Groessler, 2017-03-08
;

                .include        "zeropage.inc"

                .include        "joy-kernel.inc"
                .include        "joy-error.inc"
                .include        "creativision.inc"

                .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

               module_header   _creativisionstd_joy

; Driver signature

                .byte   $6A, $6F, $79           ; "joy"
                .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

                .addr   $0000

; Jump table.

                .addr   INSTALL
                .addr   UNINSTALL
                .addr   COUNT
                .addr   READJOY

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       =       2                       ; Number of joysticks we support

; Symbolic names for joystick masks (similar names like the defines in joystick.h, but not related to them)

JOY_UP          =       $10
JOY_DOWN        =       $04
JOY_LEFT        =       $20
JOY_RIGHT       =       $08

; ------------------------------------------------------------------------
; Code

                .code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:        lda     #JOY_ERR_OK
                ldx     #0
;               rts                             ; Fall through

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:      rts


; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;

COUNT:          lda     #<JOY_COUNT
                ldx     #>JOY_COUNT
                rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READJOY:        and     #1                      ; fix joystick number
                bne     READJOY_1               ; read right joystick

; Read left joystick

                ldx     ZP_JOY0_DIR
                lda     ZP_JOY0_BUTTONS
                jmp     convert                 ; convert joystick state to cc65 values

; Read right joystick

READJOY_1:      ldx     ZP_JOY1_DIR
                lda     ZP_JOY1_BUTTONS
                lsr     a
                lsr     a
                ;jmp    convert                 ; convert joystick state to cc65 values
                                                ; fall thru...

; ------------------------------------------------------------------------
; convert: make runtime lib compatible values
;       inputs:
;               A - buttons
;               X - direction
;

convert:

; ------
; buttons:
; Port values are for the left hand joystick (right hand joystick
; values were shifted to the right to be identical).
; Why are there two bits indicating a pressed trigger?
; According to the "Second book of programs for the Dick Smith Wizard"
; (pg. 88ff), the left hand button gives the value of
; %00010001 and the right hand button gives %00100010
; Why two bits? Can there be cases that just one of those bits is set?
; Until these questions have been answered, we only use the lower two
; bits and ignore the upper ones...

                and     #%00000011              ; button status came in in A, strip high bits
                sta     retval                  ; initialize 'retval' with button status

; ------
; direction:
; CV has a 16-direction joystick
;
; port values: (compass points)
; N      -  $49 - %01001001
; NNE    -  $48 - %01001000
; NE     -  $47 - %01000111
; ENE    -  $46 - %01000110
; E      -  $45 - %01000101
; ESE    -  $44 - %01000100
; SE     -  $43 - %01000011
; SSE    -  $42 - %01000010
; S      -  $41 - %01000001
; SSW    -  $40 - %01000000
; SW     -  $4F - %01001111
; WSW    -  $4E - %01001110
; W      -  $4D - %01001101
; WNW    -  $4C - %01001100
; NW     -  $4B - %01001011
; NNW    -  $4A - %01001010
; center -  $00 - %00000000
;
; mapping to cc65 definitions (4-direction joystick with 8 possible directions thru combinations)
; N, E, S, W            ->      JOY_UP, JOY_RIGHT, JOY_DOWN, JOY_LEFT
; NE, SE, SW, NW        ->      (JOY_UP | JOY_RIGHT), (JOY_DOWN | JOY_RIGHT), (JOY_DOWN | JOY_LEFT), (JOY_UP | JOY_LEFT)
; NNE, ENE, ESE, SSE, SSW, WSW, WNW, NNW:
;  toggle between straight and diagonal direction for every call, e.g.
;  NNE:
;    call to READJOY:   return JOY_UP | JOY_RIGHT
;    call to READJOY:   return JOY_UP
;    call to READJOY:   return JOY_UP | JOY_RIGHT
;    call to READJOY:   return JOY_UP
;    call to READJOY:   return JOY_UP | JOY_RIGHT
;    etc...

                txa                             ; move direction status into A
                beq     done                    ; center position (no bits are set), nothing to do

                and     #$0F                    ; get rid of the "$40" bit
                bit     bit0                    ; is it a "three letter" direction (NNE, ENE, etc.)?
                beq     special                 ; yes (bit #0 is zero)

                lsr     a                       ; create index into table
                tax
                lda     dirtable,x
done:           ora     retval                  ; include "button" bits
                ldx     #0
                rts

; NNE, ENE, ESE, SSE, SSW, WSW, WNW, NNW

special:        lsr     a
                tax

                lda     toggler                 ; toggle the toggler
                eor     #$01
                sta     toggler
                bne     spec_1                  ; toggler is 1, use spectable_1 entry

                lda     spectable_0,x           ; toggler is 0, use spectable_0 entry
                bne     done                    ; jump always

spec_1:         lda     spectable_1,x
                bne     done                    ; jump always

; ------------------------------------------------------------------------
;
                .rodata

                ; a mapping table of "port values" to "cc65 values"
                ; port value had been shifted one bit to the right (range 0..7)
dirtable:       .byte   JOY_DOWN                ; S
                .byte   JOY_DOWN | JOY_RIGHT    ; SE
                .byte   JOY_RIGHT               ; E
                .byte   JOY_UP   | JOY_RIGHT    ; NE
                .byte   JOY_UP                  ; N
                .byte   JOY_UP   | JOY_LEFT     ; NW
                .byte   JOY_LEFT                ; W
                .byte   JOY_DOWN | JOY_LEFT     ; SW

                ; two "special" mapping tables for three-letter directions (NNE, etc.)
spectable_0:    .byte   JOY_DOWN                ; SSW
                .byte   JOY_DOWN                ; SSE
                .byte   JOY_RIGHT               ; ESE
                .byte   JOY_RIGHT               ; ENE
                .byte   JOY_RIGHT               ; NNE
                .byte   JOY_UP                  ; NNW
                .byte   JOY_LEFT                ; WNW
                .byte   JOY_LEFT                ; WSW

spectable_1:    .byte   JOY_DOWN | JOY_LEFT     ; SSW
                .byte   JOY_DOWN | JOY_RIGHT    ; SSE
                .byte   JOY_DOWN | JOY_RIGHT    ; ESE
                .byte   JOY_UP   | JOY_RIGHT    ; ENE
                .byte   JOY_UP   | JOY_RIGHT    ; NNE
                .byte   JOY_UP   | JOY_LEFT     ; NNW
                .byte   JOY_UP   | JOY_LEFT     ; WNW
                .byte   JOY_DOWN | JOY_LEFT     ; WSW

; ------------------------------------------------------------------------
;
bit0:           .byte   $01

; ------------------------------------------------------------------------
;
                .bss
toggler:        .res    1
retval:         .res    1

                .end
