;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare < for long unsigneds
;

	.export		tosulteax
       	.import	       	lcmp, boolult

tosulteax:		    
       	jsr    	lcmp 		; Set the flags
	jmp	boolult		; Convert to boolean
