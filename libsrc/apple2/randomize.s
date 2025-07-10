;
; Ullrich von Bassewitz, 07.11.2002
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "apple2.inc"

___randomize:
        ldx     RNDH            ; Use random value supplied by ROM
        lda     RNDL
        jmp     _srand          ; Initialize generator

