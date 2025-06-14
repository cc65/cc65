
    .include "ieee754.inc"

         .importzp       sp, sreg, tmp1
         .import         addysp1
         .import         addysp
         .import         popax

    .export ftosdiveax
ftosdiveax:
    ; FIXME
    rts
