;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare < for long ints
;

	.export		toslteax
       	.import	       	lcmp, boollt

toslteax:
       	jsr    	lcmp 		; Set the flags
	jmp	boollt		; Convert to boolean
