;
; void* memset (void* ptr, int c, size_t n);
; void* _bzero (void* ptr, size_t n);
; void bzero (void* ptr, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;
; NOTE: bzero will return it's first argument as memset does. It is no problem
;       to declare the return value as void, since it may be ignored. _bzero
;       (note the leading underscore) is declared with the proper return type,
;       because the compiler will replace memset by _bzero if the fill value
;       is zero, and the optimizer looks at the return type to see if the value
;       in a/x is of any use.


 	.export	_memset, _bzero, __bzero
        .import popa, popax

        .include "../inc/jumptab.inc"
        .include "../inc/geossym.inc"

_bzero:
__bzero:
	    sta r0L
	    stx r0H
    	    lda #0
    	    sta r2L            ; fill with zeros
	    beq common

_memset:
	    sta r0L
	    stx r0H
	    jsr popax
	    sta r2L
common:	    jsr popax
	    sta r1L
	    pha
	    stx r1H
	    txa
	    pha
	    jsr FillRam
	    pla			; restore ptr and return it
	    tax
	    pla
	    rts
