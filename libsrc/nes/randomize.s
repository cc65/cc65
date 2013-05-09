;
; Ullrich von Bassewitz, 2003-05-02
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "nes.inc"

__randomize:
        ldx     tickcount       ; Use tick clock
        lda     tickcount+1
        jmp     _srand          ; Initialize generator


