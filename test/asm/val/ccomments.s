.export _main

; both expressions are valid with c_comments disabled
.feature c_comments -

.org $0002
.assert 8 / * = 4, error, "8 / * should evaluate to 4"
.assert 8/* = 4, error, "8/* should evaluate to 4"
.reloc 

_main:
    lda #0
    tax
    rts
