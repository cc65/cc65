;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;
;
; int rand (void);
; void srand (unsigned seed);
;
      	.export		_rand, _srand
	.include	"../inc/jumptab.inc"

.code

_rand:
	jsr GetRandom
	pha
	jsr GetRandom
	tax
	pla
_srand:
	rts
