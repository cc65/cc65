; The linker must compute the same values as the assembler. Each line has
; the expression evaluated at link time (imported symbols) and the same
; expression evaluated by ca65 (constants). Both bytes must be equal.
; The values are chosen so that the result does not depend on the size
; of long on the host (32 bits on Windows, 64 bits on most other hosts).

.import one, sh, zero, big

; Shift count larger than the word size: 0 (ld65 used to give $40)
.byte <(one << sh),                  <(1 << 70)                 ; 00 00

; >> is a logical shift, so the result is not negative (ld65 gave 1)
.byte ((zero - 16) >> 4) < 0,        (-16 >> 4) < 0             ; 00 00

; Multiplication wraps around without undefined behavior
.byte <(big * big + 5),              <($10000 * $10000 + 5)     ; 05 05
