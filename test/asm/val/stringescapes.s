.export _main

.feature string_escapes +

value:
.byte "\101"

_main:
    lda value
    cmp #65
    bne fail
    lda #0
    tax
    rts

fail:
    lda #1
    tax
    rts
