;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long not equal
;

	.export		tosneeax
	.import		lcmp, boolne

tosneeax:
	jsr	lcmp		; Set flags
	jmp	boolne		; Convert to boolean


