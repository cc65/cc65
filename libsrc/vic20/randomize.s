;
; Ullrich von Bassewitz, 05.11.2002
;
; void randomize (void);
; /* Initialize the random number generator */
;

    	.export	       	_randomize
	.import		_srand

	.include	"vic20.inc"

_randomize:
        lda     VIC_LINES       ; Get overflow bit
        asl     a               ; Shift bit 7 into carry
        lda     VIC_HLINE       ; Get bit 1-8 of rasterline 
        rol     a               ; Use bit 0-7
        tax                     ; Use VIC rasterline as high byte
        lda     TIME            ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

