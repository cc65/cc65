;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 6
;

       	.export	      	decsp6
	.import		subysp

.proc	decsp6

	ldy   	#6
	jmp	subysp

.endproc




	
