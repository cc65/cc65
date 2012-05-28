; Stefan Haubenthal, 2012-05-06
; based on code by Twilighte
; void __fastcall__ atmos_save(const char* name, const void* start, const void* end);

	.export		_atmos_save
	.import		popax, store_filename


.proc	_atmos_save

	sei
	sta	$02ab	; file end lo
	stx	$02ac	; file end hi
	jsr	popax
	sta	$02a9	; file start lo
	stx	$02aa	; file start hi
	jsr	popax
	jsr	store_filename
	lda	#00
	sta	$02ad
	jsr	csave_bit
	cli
	rts
csave_bit:
	php
	jmp	$e92c

.endproc
