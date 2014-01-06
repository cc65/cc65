; Cartridge "header"
; (In fact, it's at the end of the cartridge, so more a "trailer".)
;
; Christian Groessler, 06-Jan-2014

.ifndef __ATARIXL__

.import		__CARTFLAGS__, cartinit, cartstart
.export		__CART_HEADER__: absolute = 1

.include	"atari.inc"

		.segment "CARTHDR"

		.word	cartstart	; start routine
		.byte	0		; must be zero
		.byte	<__CARTFLAGS__
		.word	cartinit	; init routine

.endif	; .ifndef __ATARIXL__
