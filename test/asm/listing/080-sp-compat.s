.include "zeropage.inc"

.proc   _func
        ldy     #0
        lda     (sp),y
        rts
.endproc
