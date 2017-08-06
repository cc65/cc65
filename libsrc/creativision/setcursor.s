;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; Set the cursor position

        .export         setcursor

        .include        "creativision.inc"

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

addrlo: .repeat SCREEN_ROWS,line
        .byte <($1000+(line*SCREEN_COLS))
        .endrepeat

addrhi: .repeat SCREEN_ROWS,line
        .byte >($1000+(line*SCREEN_COLS))
        .endrepeat
