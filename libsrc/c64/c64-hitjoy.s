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

; ------------------------------------------------------------------------
; Header. Includes jump table

        .segment "JUMPTABLE"

; Driver signature

        .byte   $6A, $6F, $79	; "joy"
        .byte   JOY_API_VERSION	; Driver API version number

; Button state masks (8 values)

        .byte   $01                     ; JOY_UP
        .byte   $02                     ; JOY_DOWN
        .byte   $04                     ; JOY_LEFT
        .byte   $08                     ; JOY_RIGHT
        .byte   $10                     ; JOY_FIRE
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .word   INSTALL
        .word   DEINSTALL
        .word   COUNT
        .word   READ

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 4             ; Number of joysticks we support

; ------------------------------------------------------------------------
; Data. Written in the IRQ, read by the READ routine

.bss

temp3: 	.byte 0
temp4:	.byte 0

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        sei
        lda $0314
        sta irqjmp+1
        lda $0315
        sta irqjmp+2
        lda #<pollirq
        sta $0314
        lda #>pollirq
        sta $0315
        cli

        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK

        rts

; ------------------------------------------------------------------------
; DEINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

DEINSTALL:
        sei
        lda irqjmp+1
        sta $0314
        lda irqjmp+2
        sta $0315
        cli
        rts

; ------------------------------------------------------------------------
; we must use an irq here since we need timers
; which otherwhise would conflict with system-irq
pollirq:
         ; cia 2 setup

         ldy #$00  ; port b direction
         sty $dd03 ; => input

         sty $dd05 ; cia2 timer a highbyte
         sty $dc05 ; cia1 timer a highbyte
         iny
         sty $dd04 ; cia2 timer a lowbyte
         sty $dc04 ; cia1 timer a lowbyte

         lda #%00010001
         sta $dd0e ; control register a
                   ; timer: start
                   ;        continous
                   ;        forced load
                   ; serial port: input

         ; cia 1 setup
         lda #%01010001
         sta $dc0e ; control register a
                   ; timer: start
                   ;        continous
                   ;        forced load
                   ; serial port: output


         ; read directions 3
         lda $dd01 ;read cia 2 port b
         and #$0f
         sta temp3

         ; read button 3
         lda $dd02      ;cia 2 port a
         and #%11111011 ;data direction
         sta $dd02      ;=> bit 2 input

         lda $dd00      ;read cia 2 p.A
         and #%00000100 ;check bit 2
         asl a
         asl a
         ora temp3
         sta temp3

         ; read directions 4
         lda $dd01 ;read cia 2 port b
         lsr a
         lsr a
         lsr a
         lsr a
         sta temp4

         ; read button 4
         ldx #$ff ;serial data register
         stx $dc0c;=> writing $ff causes
                  ;cia to output some
                  ;count signals at cnt1

         ldx $dd0c ;read cia 2 serial in
         beq fire  ;button press if zero

         lda temp4
         ora #%00010000
         sta temp4

fire:

         ; Default Value: $40/64 on PAL
         ;                $42/66 on NTSC
         lda #$41
         sta $dc05
         ; Default Value: $25/37 on PAL
         ;                $95/149 on NTSC
         lda #0
         sta $dc04

irqjmp:  jmp $dead

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

READ:
        tax            ; Joystick number into X
        bne joy2

         ; Read joystick 1
joy1:
        lda #$7F
        sei
        sta CIA1_PRA
        lda CIA1_PRB
        cli
        and #$1F
        eor #$1F
        rts

        ; Read joystick 2
joy2:
        dex
        bne joy3

        ; ldx	#0
        lda	#$E0
        ldy	#$FF
        sei
        sta	CIA1_DDRA
        lda	CIA1_PRA
        sty	CIA1_DDRA
        cli
        and	#$1F
        eor	#$1F
        rts

        ; Read joystick 3

joy3:
        dex
        bne joy4

        lda temp3
        eor	#$1F
        ldx #0
        rts

        ; Read joystick 4

joy4:
        lda temp4
        eor	#$1F
        ldx #0
        rts

