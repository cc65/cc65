;
; Christian Groessler, 06.11.2002
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "atari.inc"

___randomize:
        ldx     VCOUNT          ; Use vertical line counter as high byte
        lda     RTCLOK+2        ; Use clock as low byte
        jmp     _srand          ; Initialize generator

