    .export         _kbdclick1,_oups,_ping,_explode,_shoot,_zap
    .include        "telemon30.inc"

.proc _kbdclick1
	LDX #<sound_bip_keyboard
	LDY #>sound_bip_keyboard
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



; XPLAY  :=  $43
; XSOUND  :=  $44 
; XMUSIC  :=  $45 



