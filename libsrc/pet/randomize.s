;
; Ullrich von Bassewitz, 05.11.2002
;
; void randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	_randomize
	.import		_srand

	.include	"pet.inc"

_randomize:
        ldx     TIME
        lda     TIME+1          ; Use 60HZ clock
        jmp     _srand          ; Initialize generator

