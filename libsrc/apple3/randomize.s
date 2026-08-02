;
; Robert Justice, 2026
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize
        .import         _srand

        .include        "apple3.inc"

___randomize:
        ldx     D_TIMER2C_L     ; Use VIA timer values
        lda     E_TIMER2C_L 
        jmp     _srand          ; Initialize generator

