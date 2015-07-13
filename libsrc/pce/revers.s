
; FIXME: actual revers output is not supported yet

            .export _revers
_revers:
            lda #0
            rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init = initconio
