
Per CPU, a test binary is produced (using the assembler), which should contain
all possible instructions. That file is then disassembled, and assembled again,
and finally the resulting binary compared with the binary produced in the first
step.

Given that we assume the assembler works (this is tested in other/previous
tests), this proves that the disassembler works, and can produce output that the
assembler will understand - and produce an identical binary from.
