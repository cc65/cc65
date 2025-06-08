;
; Ullrich von Bassewitz, 07.11.2002
; Oleg A. Odintsov, Moscow, 2024
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "agat.inc"

__randomize:
        ldx     RNDH            ; Use random value supplied by ROM
        lda     RNDL
        jmp     _srand          ; Initialize generator

