;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: Compare <= for long ints
;

     	.export		tosleeax
	.import		lcmp, boolle

tosleeax:
       	jsr    	lcmp 		; Set the flags
	jmp	boolle		; Convert to boolean

