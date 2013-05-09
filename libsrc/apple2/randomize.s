;
; Ullrich von Bassewitz, 07.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "apple2.inc"

__randomize:
        ldx     RNDH            ; Use random value supplied by ROM
        lda     RNDL
        jmp     _srand          ; Initialize generator

