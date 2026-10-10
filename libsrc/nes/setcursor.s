;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Rewritten by Brandon Woodward
;
; Set the cursor position and screen position

        .export         setcursor_pos, setcursor_posx, setcursor_screenptr

        .include        "nes.inc"

;-----------------------------------------------------------------------------

;y+(++x*32)+$2000
setcursor_pos:

        stx CURS_Y      ; Fallthrough

setcursor_posx:

        sty CURS_X      ; Fallthrough

.proc   setcursor_screenptr

        inx                      ; Add 1 to Y (first row is normally hidden by overscan)
        txa
        sec                      ; Set bit $20 of address MSB
        ror     a
        ror     a
        ror     a
        sta     SCREEN_PTR+1     ; Write %--1000yy (y = 2 highest bits, - = ignored)
        and     #%11000000       ; Upper 2 bits and carry contain lo bits of Y
        ror     a
        sty     SCREEN_PTR
        ora     SCREEN_PTR       ; Add X
        sta     SCREEN_PTR       ; Write %yyyxxxxx (y = 3 lowest bits)
        rts

.endproc
