
.code

; $00 - $1f control codes
.if CTRLCODES = 1
.repeat $20, n
.byte n
.endrep
.endif

; $20 - $3f special letters and numbers ( !"#$%%´()*+,-./0123456789:;<=>?)
.repeat $20, n
.byte $20 + n
.endrep

; $40 - $5f uppercase letters (@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_)
.repeat $20, n
.byte $40 + n
.endrep

; $60 - $7f lowercase letters (`abcdefghijklmnopqrstuvwxyz{|}~)
.repeat $1f, n
.byte $60 + n
.endrep

; $7f       control codes (delete)
.if CTRLCODES = 1
.byte $7f
.endif

; $80 - $9f extended control codes
.if CTRLCODES = 1
.repeat $20, n
.byte $80 + n
.endrep
.endif

; $a0 - $bf special characters
.if NONASCII = 1
.repeat $20, n
.byte $a0 + n
.endrep
.endif

; $c0 - $df uppercase latin-1 letters
.if NONASCII = 1
.repeat $20, n
.byte $c0 + n
.endrep
.endif

; $e0 - $ff lowercase latin-1 letters
.if NONASCII = 1
.repeat $20, n
.byte $e0 + n
.endrep
.endif
