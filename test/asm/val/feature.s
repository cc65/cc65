; a simple test of every .feature

.export _main

.segment "ZEROPAGE"
zplabel:

.segment "CODE"

; exit with 0

_main:
    ; if any feature needs a runtime test,
    ; it can be added here.
    lda #0
    tax
    rts

.feature at_in_identifiers on
ident@with@at:
    rts
.feature at_in_identifiers off


.feature bracket_as_indirect
    lda [$82],y
.feature bracket_as_indirect-


.feature c_comments
    lda zplabel /* comment */
    /* comment */
/* multiline
** comment
*/
.feature c_comments -


.feature dollar_in_identifiers
ident$with$dollar:
    rts
.feature dollar_in_identifiers -


.feature dollar_is_pc
.assert $ = *, error, "dollar_is_pc failure"
.feature dollar_is_pc -


.feature force_range
    lda #-1
.feature force_range -


.feature labels_without_colons
labelwithoutcolon
    jmp labelwithoutcolon
.feature labels_without_colons -


.feature leading_dot_in_identifiers
.identifierwithdot:
    rts
.feature leading_dot_in_identifiers -


.feature long_jsr_jmp_rts
.p816
    ; long addresses require alternate instruction names JSL, JML without this feature
    jsr $123456
    jmp $123456
; smart + far + long_jsr_jmp_rts will promote rts to rtl
.smart +
.proc long_rts : far
    rts ; should become RTL ($6B) instead of RTS ($60)
    ; the emitted opcode is not verified by this test,
    ; see test/asm/listing/108-long-rts
.endproc
.smart -
.p02
.feature long_jsr_jmp_rts -


.feature loose_char_term
.byte 'a'
.byte "a"
.feature loose_char_term -


.feature loose_string_term
.asciiz "string"
.asciiz 'string'
.feature loose_string_term -


.feature missing_char_term
    lda #'a
.feature missing_char_term -


.feature org_per_seg
.segment "RODATA"
.org $5678
.assert * = $5678, error, "org_per_seg failed"
.segment "CODE"
.org $9ABC
.assert * = $9ABC, error, "org_per_seg failed"
.segment "RODATA"
.assert * = $5678, error, "org_per_seg failed"
.reloc
.segment "CODE"
.assert * = $9ABC, error, "org_per_seg failed"
.reloc
.feature org_per_seg -


.feature pc_assignment
* = $1234
.assert * = $1234, error, "pc_assignment failed"
.reloc
.feature pc_assignment -


.feature string_escapes
.asciiz "quote:\""
.feature string_escapes -


.feature ubiquitous_idents
.macro bit
    brk
.endmacro
    bit
.feature ubiquitous_idents -


.feature underline_in_numbers
.byte %10_10_10_10
.feature underline_in_numbers -
