;
; Ullrich von Bassewitz, 05.11.2002
;
; void randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	_randomize
	.import		_srand

	.include	"c64.inc"

_randomize:
        ldx     VIC_HLINE       ; Use VIC rasterline as high byte
        lda     TIME            ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

