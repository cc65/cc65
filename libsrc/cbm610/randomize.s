;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand
        .importzp       time

___randomize:
        ldx     time+2          ; Use 50/60HZ clock
        lda     time+1
        jmp     _srand          ; Initialize generator

