;
; Stefan Haubenthal, Jul 10 2003
;
; unsigned char get_ostype(void)
;
; $AA US
; $64 PET-64
; $43 SX-64
; $03 EU_NEW
; $00 EU_OLD
;

	.export		_get_ostype

.proc	_get_ostype

        ldx     #$00            ; Clear high byte
	lda	$ff80
	rts

.endproc


