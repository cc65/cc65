; test that jmp (indirect) on a page boundary will give an error for 6502 CPU

.p02
jmp ($10FF)
