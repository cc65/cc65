;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 7
;

       	.export	      	decsp7
	.import		subysp

.proc	decsp7

	ldy   	#7
	jmp	subysp

.endproc




	
