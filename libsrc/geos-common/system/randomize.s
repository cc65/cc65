;
; Ullrich von Bassewitz, 05.11.2002
;
; void __randomize (void);
; /* Initialize the random number generator */
;

            .export ___randomize
            .import _srand

            .include "geossym.inc"

___randomize:
        lda random              ; get random value from internal generator
        ldx random+1
        jmp _srand              ; and use it as seed
