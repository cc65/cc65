.export _main

; both expressions are valid with c_comments disabled
.feature c_comments -
.word 8 / *
.word 8/*  

_main:
    lda #0
    tax
    rts
