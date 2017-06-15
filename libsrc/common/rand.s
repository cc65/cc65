;
; Randum number generator
;
; Written and donated by Sidney Cadot - sidney@ch.twi.tudelft.nl
; 2016-11-07, modified by Brad Smith
;
; May be distributed with the cc65 runtime using the same license.
;
;
; int rand (void);
; void srand (unsigned seed);
;
;  Uses 4-byte state.
;  Multiplier must be 1 (mod 4)
;  Added value must be 1 (mod 2)
;  This guarantees max. period (2**32)
;  The lowest bits have poor entropy and
;  exhibit easily detectable patterns, so
;  only the upper bits 16-22 and 24-31 of the
;  4-byte state are returned.
;
;  The best 8 bits, 24-31 are returned in the
;  low byte A to provide the best entropy in the
;  most commonly used part of the return value.
;

        .export         _rand, _srand

.data

; The seed. When srand() is not called, the C standard says that that rand()
; should behave as if srand() was called with an argument of 1 before.
rand:   .dword   1

.code

_rand:  clc
        lda     rand+0          ; SEED *= $01010101
        adc     rand+1
        sta     rand+1
        adc     rand+2
        sta     rand+2
        adc     rand+3
        sta     rand+3
        clc
        lda     rand+0          ; SEED += $31415927
        adc     #$27
        sta     rand+0
        lda     rand+1
        adc     #$59
        sta     rand+1
        lda     rand+2
        adc     #$41
        sta     rand+2
        and     #$7f            ; Suppress sign bit (make it positive)
        tax
        lda     rand+3
        adc     #$31
        sta     rand+3
        rts                     ; return bit (16-22,24-31) in (X,A)

_srand: sta     rand+0          ; Store the seed
        stx     rand+1
        lda     #0
        sta     rand+2          ; Set MSW to zero
        sta     rand+3
        rts


