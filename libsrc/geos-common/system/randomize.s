;
; Ullrich von Bassewitz, 05.11.2002
;
; void _randomize (void);
; /* Initialize the random number generator */
;

            .export __randomize
            .import _srand

            .include "geossym.inc"

__randomize:
        lda random              ; get random value from internal generator
        ldx random+1
        jmp _srand              ; and use it as seed
