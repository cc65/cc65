;
; Ullrich von Bassewitz, 05.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	__randomize
	.import		_srand

	.include	"pet.inc"

__randomize:              
        ldx     TIME
        lda     TIME+1          ; Use 60HZ clock
        jmp     _srand          ; Initialize generator

