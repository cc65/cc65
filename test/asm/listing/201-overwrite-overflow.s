; verification of overwrite segment overflow cases

; error: overflow past end of A memory area
.segment "A"
.byte 0,1,2,3
.segment "AO"
.byte $26,$27,$28

; error: overflow past end of B memory area
.segment "B"
.byte 0,1,2,3
.segment "BO"
.byte $26,$27,$28
