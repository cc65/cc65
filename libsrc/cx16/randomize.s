;
; 2020-05-02, Greg King
;
; void __randomize (void);
; /* Initialize the random number generator */
;

        .export         ___randomize

        .import         ENTROPY_GET, _srand

___randomize:
        jsr     ENTROPY_GET
        jmp     _srand          ; Initialize generator
