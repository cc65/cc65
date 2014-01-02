;
; Cassette boot file header
;
; Christian Groessler, chris@groessler.org, 2013
;

.ifndef __ATARIXL__

	.include "atari.inc"

	.import	__BSS_RUN__, __STARTADDRESS__, start
	.export	_cas_init

.segment        "CASHDR"

	.byte	0	; ignored
	.byte	<((__BSS_RUN__ - __STARTADDRESS__ + 127 + 6) / 128)
	.word	__STARTADDRESS__
	.word	_cas_init

.segment	"CASINIT"

	lda	#33
	ldy	#80
	sta	(SAVMSC),y
	clc
	rts

_cas_init:
	lda	#34
	ldy	#81
	sta	(SAVMSC),y

	lda	#<start
	sta	DOSVEC
	lda	#>start
	sta	DOSVEC+1
	rts

.endif	; .ifdef __ATARIXL__
