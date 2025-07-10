.ifndef __APPLE2ENH__

        .constructor    initmachinetype, 8

        .import         ostype
        .export         machinetype

        .segment        "ONCE"

initmachinetype:
        ldx     ostype
        cpx     #$31          ; Apple //e enhanced?
        ror     machinetype   ; Carry to high bit
        cpx     #$30          ; Apple //e?
        ror     machinetype
        rts

        .data

; bit 7: Machine is a //e or newer
; bit 6: Machine is a //e enhanced or newer
machinetype:            .byte 0

.endif
