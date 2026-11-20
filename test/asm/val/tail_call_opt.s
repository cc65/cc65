; test of the tail_call_opt feature: JSR abs directly followed by RTS is
; assembled as JMP abs. The sizes are checked with .assert, the run time
; behavior is checked by calling through the merged sequence.

.export _main

.macro callret target
    jsr target
    rts
.endmacro

.segment "DATA"
counter: .byte 0

.segment "CODE"

; ---- run time check: a merged JSR/RTS returns to the caller of f ----

_main:
    lda #0
    sta counter
    jsr func
    lda counter
    cmp #1
    bne fail
    lda #0
    tax
    rts
fail:
    lda #1
    ldx #0
    rts

func: jsr sub_g    ; becomes JMP g, g returns directly to _main
    rts
sub_g: inc counter
    rts

; ---- size checks ----

target: nop

; The feature is off by default: JSR + RTS = 4 bytes
off_start:
    jsr target
    rts
off_end:
.assert off_end - off_start = 4, error, "tail_call_opt must be off by default"

.feature tail_call_opt +

on_start:
    jsr target
    rts
on_end:
.assert on_end - on_start = 3, error, "JSR/RTS not merged"

; a label in front of the RTS prevents it, whatever kind of label
lab_start:
    jsr target
named: rts
lab_end:
.assert lab_end - lab_start = 4, error, "named label on RTS ignored"

cheap_start:
    jsr target
@cheap: rts
cheap_end:
.assert cheap_end - cheap_start = 4, error, "cheap local label on RTS ignored"

anon_start:
    jsr target
:   rts
anon_end:
.assert anon_end - anon_start = 4, error, "anonymous label on RTS ignored"
    jmp :-          ; never executed, references the unnamed label

; a label on the JSR is fine
lj_start:
ljlabel: jsr target
    rts
lj_end:
.assert lj_end - lj_start = 3, error, "label on JSR must not matter"

; anything in between prevents it
data_start:
    jsr target
    .byte $EA
    rts
data_end:
.assert data_end - data_start = 5, error, "data between JSR and RTS ignored"

nop_start:
    jsr target
    nop
    rts
nop_end:
.assert nop_end - nop_start = 5, error, "instruction between JSR and RTS ignored"

; can be switched off again
.feature tail_call_opt -
region_start:
    jsr target
    rts
region_end:
.assert region_end - region_start = 4, error, "tail_call_opt - ignored"
.feature tco +

; also works on macro expansions and with the alias
mac_start:
    callret target
mac_end:
.assert mac_end - mac_start = 3, error, "macro expansion not merged"

; forward references are merged, too
fwd_start:
    jsr later
    rts
fwd_end:
.assert fwd_end - fwd_start = 3, error, "forward reference not merged"
later: nop

.feature tail_call_opt -
