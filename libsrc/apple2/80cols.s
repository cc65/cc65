.ifndef __APPLE2ENH__

        .export         has_80cols_card

        .data

; Export a single byte, so that if the user is not interested in
; videomode(), we won't load any unnecessary code.
has_80cols_card:        .byte 0

.endif
