;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand
        .importzp       time

__randomize:
        ldx     time+2          ; Use 50/60HZ clock
        lda     time+1
        jmp     _srand          ; Initialize generator

