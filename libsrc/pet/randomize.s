;
; 2002-11-05, Ullrich von Bassewitz
; 2015-09-11, Greg King
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "pet.inc"

__randomize:              
        ldx     TIME+2
        lda     TIME+1          ; Use 60HZ clock
        jmp     _srand          ; Initialize generator

