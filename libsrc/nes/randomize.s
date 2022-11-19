;
; Ullrich von Bassewitz, 2003-05-02
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "nes.inc"

___randomize:
        ldx     tickcount       ; Use tick clock
        lda     tickcount+1
        jmp     _srand          ; Initialize generator


