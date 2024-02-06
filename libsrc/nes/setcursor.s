;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; Set the cursor position

        .export         setcursor

        .include        "nes.inc"

;-----------------------------------------------------------------------------

.proc   setcursor

        tya
        clc
        adc     addrlo,x
        sta     SCREEN_PTR

        lda     addrhi,x
        adc     #0
        sta     SCREEN_PTR+1
        rts

.endproc

;-----------------------------------------------------------------------------
; Tables with screen addresses

addrlo: .repeat screenrows,line
        .byte <($2000+((1*32))+(line*charsperline))
        .endrepeat

addrhi: .repeat screenrows,line
        .byte >($2000+((1*32))+(line*charsperline))
        .endrepeat

