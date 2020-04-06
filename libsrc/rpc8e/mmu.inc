.macro mmu command
	.byte $EF
	.byte command
.endmacro