;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare > for long ints
;

 	.export		tosgteax
 	.import		lcmp, boolgt

tosgteax:
       	jsr    	lcmp		; Set the flags
	jmp	boolgt		; Convert to boolean


