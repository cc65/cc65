;
; Ullrich von Bassewitz, 10.12.1998
;
; CC65 runtime: Compare > for long unsigneds
;

 	.export		tosugteax
 	.import		lcmp, boolugt

tosugteax:		    
       	jsr    	lcmp	    	; Set the flags
	jmp	boolugt        	; Convert to boolean


