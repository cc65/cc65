
.code

; $00 - $1f control codes
.if CTRLCODES = 1
.repeat $20, n
.byte n
.endrep
.endif

; $20 - $5a alphanumeric ( !"#$%&´()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz)
.repeat $3b, n
.byte $20 + n
.endrep
; $5b - $5f alphanumeric (left square bracket, pound, right square bracket, arrow up, arrow left)
.byte $5b, $5c, $5d, $5e, $5f

; $60       gfx chars
.if GFXCODES = 1
.byte $60
.endif

; $61 - $7a alphanumeric (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
.repeat $1a, n
.byte $61 + n
.endrep

; $7b - $7d gfx chars
.if GFXCODES = 1
.byte $7b, $7c, $7d
.endif
; $7e       pi
.byte $7e
; $7f       gfx chars
.if GFXCODES = 1
.byte $7f
.endif

; $80 - $9f control codes
.if CTRLCODES = 1
.repeat $20, n
.byte $80 + n
.endrep
.endif

; $a0 - $c0 gfx chars
.if GFXCODES = 1
.repeat $21, n
.byte $a0 + n
.endrep
.endif

; $c1 - $da alphanumeric (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
.repeat $1a, n
.byte $c1 + n
.endrep

; $db - $fe gfx chars
.if GFXCODES = 1
.repeat $24, n
.byte $db + n
.endrep
.endif

; $ff       pi
.byte $ff



