;
; void* memset (void* ptr, int c, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;

 	.export	_memset
        .import popa, popax

        .include "../inc/jumptab.inc"
        .include "../inc/geossym.inc"

_memset:
	    sta r0L
	    stx r0H
	    jsr popax
	    sta r2L
	    jsr popax
	    sta r1L
	    stx r1H
	    jmp FillRam
