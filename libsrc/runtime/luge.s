;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare >= for long unsigneds
;

	.export		tosugeeax
       	.import	       	lcmp, booluge

tosugeeax:		    
       	jsr    	lcmp		; Set the flags
       	jmp    	booluge		; Convert to boolean

