;
; Robert Justice, 2026
;
; signed char __fastcall__ videomode (unsigned mode);
;
        .export  _videomode
        .import  _cputc, consvpwidth, consvpheight

        .include        "apple3.inc"

_videomode:
        pha                             ; save mode

        ldy     #80
        cmp     #VIDEOMODE_80x24
        beq     :+
        ldy     #40
:       sty     consvpwidth
        ldy     #24
        sty     consvpheight

        lda     #CONSOLE_TEXT_MODE
        jsr     _cputc
        pla
        jsr     _cputc
        rts
