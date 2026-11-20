; test of ca65 -E: macros are expanded, conditionals are resolved, repeats are
; unrolled. The expanded source must assemble to the same code as the
; original.

.macro twice instr
    instr
    instr
.endmacro

.macro loadwith value
    .if value = 0
        lda #0
    .else
        lda #<value
        ldx #>value
    .endif
.endmacro

.macro with_local
    .local skip
    bne skip
    inx
skip:
.endmacro

.macro text str
    .byte str, 0
.endmacro

start:
    twice nop
    loadwith 0
    loadwith $1234
    with_local
    with_local
    .repeat 3, i
        ldy #i
    .endrepeat
    jsr sub
@cheap:
    bne @cheap
    text "hello"
    text .sprintf ("a%cb", 34)
    rts
sub:
    rts
