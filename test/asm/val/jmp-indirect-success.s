; test that jmp (indirect) on a page boundary will not give an error for non-6502 CPUs

.pc02
jmp ($10FF)

.psc02
jmp ($10FF)

.p816
jmp ($10FF)

; main always returns success (the tested issue is only whether the assembly errors)
.import _exit
.export _main
_main:
    lda #0
    tax
    jmp _exit
