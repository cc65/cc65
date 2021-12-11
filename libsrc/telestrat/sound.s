;
; jede jede@oric.org 2017-01-22

    .export         _kbdclick1,_oups,_ping,_explode,_shoot,_zap
    .include        "telestrat.inc"

.proc _kbdclick1
    ldx #<sound_bip_keyboard
    ldy #>sound_bip_keyboard
    BRK_TELEMON XSONPS
    rts
sound_bip_keyboard:
    .byte $1f,$00,$00,$00,$00,$00,$00,$3e,$10,$00,$00,$1f,$00,$00
.endproc

.proc _explode
    BRK_TELEMON XEXPLO
    rts
.endproc

.proc _oups
    BRK_TELEMON XOUPS
    rts
.endproc

.proc _ping
    BRK_TELEMON XPING
    rts
.endproc

.proc _shoot
    BRK_TELEMON XSHOOT
    rts
.endproc

.proc _zap 
    BRK_TELEMON XZAP
    rts
.endproc



