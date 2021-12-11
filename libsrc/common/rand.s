;
; Randum number generator
;
; Written and donated by Sidney Cadot - sidney@ch.twi.tudelft.nl
; 2016-11-07, modified by Brad Smith
; 2019-10-07, modified by Lewis "LRFLEW" Fox
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
;  The quality of entropy in the bits of the seed are poorest in the lowest
;  bits, and best in the highest bits.
;
;  The high 8 bits are used for the low byte A to provide the best entropy in
;  the most commonly used part of the return value.
;
;  Finally XOR with the lower 2 bytes is used on the output, which breaks up
;  some minor deficient sequential patterns. (#951)
;
;  Uses the following LCG values for ax + c (mod m)
;  a = $01010101
;  c = $B3B3B3B3
;  m = $100000000 (32-bit truncation)
;
;  The multiplier was carefully chosen such that it can
;  be computed with 3 adc instructions, and the increment
;  was chosen to have the same value in each byte to allow
;  the addition to be performed in conjunction with the
;  multiplication, adding only 1 additional adc instruction.
;

        .export         _rand, _srand

.data

; The seed. When srand() is not called, the C standard says that that rand()
; should behave as if srand() was called with an argument of 1 before.
rand:   .dword $B5B5B4B4

.code

_srand: sta     rand+0          ; Store the seed
        stx     rand+1
        sta     rand+2          ; argument << 16 is convenient fill for MSW
        stx     rand+3
        ; fall through to rand() to sufficiently "shuffle" first rand() result

_rand:  clc
        lda     rand+0
        adc     #$B3
        sta     rand+0
        adc     rand+1
        sta     rand+1
        adc     rand+2
        sta     rand+2
        eor     rand+0
        and     #$7f            ; Suppress sign bit (make it positive)
        tax
        lda     rand+2
        adc     rand+3
        sta     rand+3
        eor     rand+1
        rts                     ; return bit (16-22,24-31) in (X,A)
