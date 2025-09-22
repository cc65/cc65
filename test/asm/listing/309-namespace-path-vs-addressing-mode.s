.org $2000

.proc s1

    symbol := $1

    stx     s1::s2::symbol
    stx     s2_symbol

.scope s2
symbol:    .byte   0
.endscope

s2_symbol := s2::symbol

.endproc
