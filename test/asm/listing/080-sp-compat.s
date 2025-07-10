.include "zeropage.inc"

; FIXME: there must be a less ugly way to do this
.ifp4510
.else
.ifp45GS02
.else

.proc   _func
        ldy     #0
        lda     (sp),y
        rts
.endproc

.endif
.endif
