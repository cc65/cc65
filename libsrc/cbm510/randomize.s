;
; Ullrich von Bassewitz, 05.11.2002
;
; void randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	_randomize
	.import		_srand
        .importzp       time

_randomize:
        ldx     time            ; Use 50/60HZ clock
        lda     time+1
        jmp     _srand          ; Initialize generator

