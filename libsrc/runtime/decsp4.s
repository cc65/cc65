;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 4
;

       	.export	      	decsp4
	.import		subysp

.proc	decsp4

	ldy	#4
	jmp	subysp

.endproc




	
