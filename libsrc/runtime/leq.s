;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: long equal
;

	.export		toseqeax
	.import		lcmp, booleq

toseqeax:
     	jsr	lcmp  		; Set flags
     	jmp	booleq 	       	; Convert to boolean


     
