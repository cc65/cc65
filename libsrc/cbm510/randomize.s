;
; Ullrich von Bassewitz, 05.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	__randomize
	.import		_srand
        .importzp       time

__randomize:
        ldx     time            ; Use 50/60HZ clock
        lda     time+1
        jmp     _srand          ; Initialize generator

