;
; Ullrich von Bassewitz, 05.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	__randomize
	.import		_srand

	.include	"plus4.inc"

__randomize:
        ldx     TED_VLINELO     ; Use TED rasterline as high byte
        lda     TIME            ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

