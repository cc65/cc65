;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 5
;

       	.export	      	decsp5
	.import		subysp

.proc	decsp5

	ldy   	#5
	jmp	subysp

.endproc




	
