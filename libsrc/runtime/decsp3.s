;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 3
;

       	.export	  	decsp3
	.import		subysp

.proc	decsp3

	ldy	#3
	jmp	subysp

.endproc




	
