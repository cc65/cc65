;
; Christian Groessler, 06.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize
        .import         _srand

        .include        "atari.inc"

__randomize:              
        ldx     VCOUNT          ; Use vertical line counter as high byte
        lda     RTCLOK+2        ; Use clock as low byte
        jmp     _srand          ; Initialize generator

