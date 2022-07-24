;
; 2020-05-02, Greg King
;
; void _randomize (void);
; /* Initialize the random number generator */
;

        .export         __randomize

        .import         ENTROPY_GET, _srand

__randomize:
        jsr     ENTROPY_GET
        jmp     _srand          ; Initialize generator
