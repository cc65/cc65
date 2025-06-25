;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "c65.inc"

___randomize:
        ldx     VIC_HLINE       ; Use VIC rasterline as high byte
        lda     TIME+2          ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

