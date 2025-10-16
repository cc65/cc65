.org $800

.scope main
.proc foo

    ;; should all be the same
    lda params__bar
    lda params::bar
    lda foo::params::bar

.proc params
bar: .byte 0
.endproc
    params__bar := params::bar

.endproc
.endscope
