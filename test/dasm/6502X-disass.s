
    .setcpu "6502X"

    .repeat 256, cnt

    ; generate a pattern with all opcodes. however, for the full cycle to work,
    ; we must take care of the "duplicate" opcodes, ie use only the favourite one.
    .if ((cnt & $0f) = $02)
    .byte $02   ; all JAM
    .elseif ((cnt & $1f) = $1a)
    .byte $ea   ; all NOP
    .elseif (cnt = $2b)
    .byte $0b   ; both ANC #imm
    .elseif (cnt = $89)
    .byte $80   ; both NOP #imm
    .elseif (cnt = $eb)
    .byte $e9   ; both SBC #imm
    .elseif (cnt = $34) || (cnt = $54) ||  (cnt = $74) ||  (cnt = $d4) || (cnt = $f4)
    .byte $14   ; all NOP zp, x
    .elseif (cnt = $3c) || (cnt = $5c) ||  (cnt = $7c) ||  (cnt = $dc) || (cnt = $fc)
    .byte $1c   ; all NOP abs, x
    .elseif (cnt = $44) || (cnt = $64)
    .byte $04   ; all NOP zp
    .else
    .byte cnt
    .endif

    .byte $02, $ea, $00

    .endrepeat
