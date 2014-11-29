;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: call function via pointer in ax
;

	.export		callax

.code

callax:
    sta	vec
	stx	vec+1
	jmp	(vec)     	; jump there

.bss

vec:
    .res 2