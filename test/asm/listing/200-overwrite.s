; verification of overwrite segment feature
; See: https://github.com/cc65/cc65/issues/1366

; A: full memory area which is overwritten to the end
.segment "A"
.byte 0,1,2,3,4,5,6,7
.segment "AO"
.byte $24,$25,$26,$27

; B: incomplete memory area overwritten in the fill area
.segment "B"
.byte 0,1,2
.segment "BO"
.byte $25,$26

; C: memory area with gap overwritten across the gap
.segment "C0"
.byte 0,1,2
.segment "C1"
.byte 5,6,7
.segment "CO"
.byte $22,$23,$24,$25

; D: incomplete memory area without fill,
;    but overwrite extends past existing segments
.segment "D"
.byte 0,1,2
.segment "DO"
.byte $24,$25
