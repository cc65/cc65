; val_asm.s: Test assembly symbols and segments

        .export     _asm_func, _main

        .segment    "DATA"
var1:   .byte       $12
var2:   .word       $1234
        
        .segment    "CODE"

; type=lab, seg
_asm_func:
        lda     var1
        sta     var2
        rts

_main:
        jmp     _asm_func

; type=equ
CONST_VAL = $55

; bank
        .segment "BANKED"
        .byte   $42

; zeropage
        .zeropage
zp_var: .res    1

