;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "c128.inc"

__randomize:
        ldx     VIC_HLINE       ; Use VIC rasterline as high byte
        lda     TIME+2          ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

