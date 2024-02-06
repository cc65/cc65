;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "vic20.inc"

___randomize:
        lda     VIC_LINES       ; Get overflow bit
        asl     a               ; Shift bit 7 into carry
        lda     VIC_HLINE       ; Get bit 1-8 of rasterline
        rol     a               ; Use bit 0-7
        tax                     ; Use VIC rasterline as high byte
        lda     TIME+2          ; Use 60HZ clock as low byte
        jmp     _srand          ; Initialize generator

