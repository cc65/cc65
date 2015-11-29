;
; original lcdtest.s by PeT (mess@utanet.at)
;
; cl65 -t gamate -o lcdtest.bin lcdtest.s
;

        .include "gamate.inc"

        .zeropage
addr:           .word 0
psa:            .word 0

        .bss
temp_x:         .byte 0
temp_y:         .byte 0
temp_a:	        .byte 0
irq_count:      .byte 0
nmi_count:      .byte 0
psx:            .byte 0
psy:            .byte 0
count:          .word 0
counted:        .word 0
xpos:           .byte 0
ypos:           .byte 0

        .rodata

chars:          .incbin "cga2.chr"

hex2asc:        .byte "0123456789abcdef"
format:         .byte "IrqNmiCountXposYpos", 0
xdesc:          .byte "0123456789abcdefghijklmnopqrstuv", 0
ydesc:          .byte "0123456789ABCDEFGHIJKLMNOPQRSTUV", 0

        .code

;-------------------------------------------------------------------------------
        .export IRQStub, NMIStub


.proc   NMIStub

        inc     nmi_count
        rts
.endproc

.proc   IRQStub

        inc     irq_count
        lda     count
        sta     counted
        lda     count+1
        sta     counted+1
        lda     #0
        sta     count
        sta     count+1
        rts
.endproc

;-------------------------------------------------------------------------------
        .export Start

.proc   Start
        sei
        lda     #0
        sta     ZP_IRQ_CTRL

        lda #0
        sta     LCD_XPOS
        sta     LCD_YPOS
        cli
        lda     #LCD_MODE_INC_Y
        sta     LCD_MODE

        lda     #0
        sta     LCD_X
        lda     #<xdesc
        ldx     #>xdesc
        ldy     #0
        jsr     printstring

        lda     #LCD_XPOS_PLANE2
        sta     LCD_X
        lda     #<xdesc
        ldx     #>xdesc
        ldy     #128
        jsr     printstring

        lda     #0
        sta     LCD_X
        lda     #<ydesc
        ldx     #>ydesc
        ldy     #0
        sty     LCD_Y
        jsr     printstringy

        lda     #(LCD_XPOS_PLANE2|(128/8))          ; ???
        sta     LCD_X
        lda     #<ydesc
        ldx     #>ydesc
        ldy     #0
        sty     LCD_Y
        jsr     printstringy

        lda     #<format
        ldx     #>format
        ldy     #8
        jsr     printstring

        lda     #0
        sta     LCD_MODE
        lda     #24/8
        sta     LCD_X
        lda     #24
        sta     LCD_Y
        lda     #'X'
        jsr     printsign

        lda     #$80
        sta     LCD_MODE
        lda     #32/8
        sta     LCD_X
        lda     #32
        sta     LCD_Y
        lda     #'Y'
        jsr     printsign

        lda     #$c0
        sta     LCD_MODE
        lda     #40/8
        sta     LCD_X
        lda     #40
        sta     LCD_Y
        lda     #'Z'
        jsr     printsign

        lda     #0
        sta     LCD_MODE
        lda     #LCD_XPOS_PLANE2|(48/8)
        sta     LCD_X
        lda     #48
        sta     LCD_Y
        lda     #'x'
        jsr     printsign

        lda     #$80
        sta     LCD_MODE
        lda     #(LCD_XPOS_PLANE2|(56/8))
        sta     LCD_X
        lda     #56
        sta     LCD_Y
        lda     #'y'
        jsr     printsign

        lda     #$c0
        sta     LCD_MODE
        lda     #(LCD_XPOS_PLANE2|(64/8))
        sta     LCD_X
        lda     #64
        sta     LCD_Y
        lda     #'z'
        jsr     printsign

        lda     #LCD_MODE_INC_Y|1
        sta     LCD_MODE
        lda     #16/8
        sta     LCD_X
        lda     #72
        sta     LCD_Y
        lda     #'V'
        jsr     printsign

        lda     #LCD_MODE_INC_Y|2
        sta     LCD_MODE
        lda     #24/8
        sta     LCD_X
        lda     #72
        sta     LCD_Y
        lda     #'V'
        jsr     printsign

        lda     #LCD_MODE_INC_Y|4
        sta     LCD_MODE
        lda     #32/8
        sta     LCD_X
        lda     #72
        sta     LCD_Y
        lda     #'V'
        jsr     printsign

        lda     #LCD_MODE_INC_Y|8
        sta     LCD_MODE
        lda     #40/8
        sta     LCD_X
        lda     #72
        sta     LCD_Y
        lda     #'V'
        jsr     printsign

        lda     #1
        sta     ZP_IRQ_CTRL

loop:
        lda     count
        clc
        adc     #1
        sta     count
        lda     count+1
        adc     #0
        sta     count+1

        lda     irq_count
        cmp     irq_count
        beq     loop

        jsr     inputs

        lda     #LCD_MODE_INC_Y
        sta     LCD_MODE
        jsr     printy

        jmp     loop
.endproc


;-------------------------------------------------------------------------------

.proc printy
        ldx     #0
        ldy     #16
        lda     irq_count
        jsr     printhex

        ldx     #3
        ldy     #16
        lda     nmi_count
        jsr     printhex

        ldx     #6
        ldy     #16
        lda     counted+1
        jsr     printhex
        ldx     #8
        ldy     #16
        lda     counted
        jsr     printhex

        ldx     #11
        ldy     #16
        lda     xpos
        jsr     printhex
        ldx     #14
        ldy     #16
        lda     ypos
        jsr     printhex
        rts
.endproc

.proc inputs
        lda     JOY_DATA
        and     #JOY_DATA_UP
        bne     notup
        dec     ypos
        lda     ypos
        sta     LCD_YPOS
notup:
        lda     JOY_DATA
        and     #JOY_DATA_DOWN
        bne     notdown
        inc     ypos
        lda     ypos
        sta     LCD_YPOS
notdown:
        lda     JOY_DATA
        and     #JOY_DATA_LEFT
        bne     notleft
        dec     xpos
        lda     xpos
        sta     LCD_XPOS
notleft:
        lda     JOY_DATA
        and     #JOY_DATA_RIGHT
        bne     notright
        inc     xpos
        lda     xpos
        sta     LCD_XPOS
notright:
        lda     JOY_DATA
        and     #JOY_DATA_START
        bne     notstart
notstart:
        lda     JOY_DATA
        and     #JOY_DATA_SELECT
        bne     notselect
notselect:
        lda     JOY_DATA
        and     #JOY_DATA_FIRE_A
        bne     notbuttona
notbuttona:
        lda     JOY_DATA
        and     #JOY_DATA_FIRE_B
        bne     notbuttonb
notbuttonb:
        rts
.endproc

;-------------------------------------------------------------------------------

.proc printstring
        sta     psa
        stx     psa+1
        ldx     #0
        stx     psx
        sty     psy
printstring2:
        ldy     #0
        lda     (psa),y
        beq     printstring1
        ldx     psx
        stx     LCD_X
        ldy      psy
        sty     LCD_Y
        jsr     printsign
        inc     psx
        lda     psa
        clc
        adc     #1
        sta     psa
        lda     psa+1
        adc     #0
        sta     psa+1
        jmp     printstring2
printstring1:
        rts
.endproc

.proc printstringy
        sta     psa
        stx     psa+1
printstring2:
        ldy     #0
        lda     (psa),y
        beq     printstring1
        jsr     printsign
        lda     psa
        clc
        adc     #1
        sta     psa
        lda     psa+1
        adc     #0
        sta     psa+1
        jmp     printstring2
printstring1:
        rts
.endproc


.proc printhex
        pha
        lsr
        lsr
        lsr
        lsr
        and     #$0f
        stx     temp_x
        tax
        lda     hex2asc,x
        ldx     temp_x
        stx     LCD_X
        sty     LCD_Y
        jsr     printsign
        pla
        and     #$0f
        inx
        stx     temp_x
        tax
        lda     hex2asc,x
        ldx     temp_x
        stx     LCD_X
        sty     LCD_Y
        jmp     printsign
.endproc

.proc printsign
        sty     temp_y
        stx     temp_x
        sta     temp_a

        lda     temp_a
        sta     addr

        lda     #0
        sta     addr+1
        asl     addr
        rol     addr+1
        asl     addr
        rol     addr+1
        asl     addr
        rol     addr+1

        lda     addr
        clc
        adc     #<chars
        sta     addr
        lda     addr+1
        adc     #>chars
        sta     addr+1

        ldx     #8
        ldy     #0
printsign1:
        lda     (addr),y
        sta     LCD_DATA
        iny
        dex
        bne     printsign1

        ldx     temp_x
        ldy     temp_y
        rts
.endproc

