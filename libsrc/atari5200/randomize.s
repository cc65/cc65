;
; Christian Groessler, 01-Mar-2014
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "atari5200.inc"

___randomize:
        ldx     VCOUNT          ; Use vertical line counter as high byte
        lda     RTCLOK+1        ; Use clock as low byte
        jmp     _srand          ; Initialize generator

