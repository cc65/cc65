;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 8
;

       	.export	      	decsp8
	.import		subysp

.proc	decsp8

	ldy   	#8
	jmp	subysp

.endproc




	
