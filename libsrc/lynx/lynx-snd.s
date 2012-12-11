;
; Sound driver for the Atari Lynx.
;
; Karri Kaksonen and Bjoern Spruck, 11.12.2012
;

	.include	"lynx.inc"
	.include	"zeropage.inc"

	.export		_lynx_snd_init

;----------------------------------------------------------------------------
; ZP variables that go into APPZP
;

	.segment "APPZP" : zeropage

SndSema:		.res	1
SndPtrTmp:		.res	2
SndTmp:			.res	2
SndEnvPtr:		.res	2

;----------------------------------------------------------------------------
; Global variables
;

	.bss

SndRetAFlag2:		.res	1
SndRetAFlag:		.res	1
SndPtrLo:		.res	4
SndPtrHi:		.res	4
SndDelay:		.res	4
SndLoopCnt:		.res	4
SndLoopPtrLo:		.res	4
SndLoopPtrHi:		.res	4
SndVolume:		.res	4
SndMaxVolume:		.res	4
SndNotePlaying:		.res	4
SndRetAddr:		.res	8
SndActive:		.res	4
SndReqStop:		.res	4
SndEnvVol:		.res	4
SndEnvFrq:		.res	4
SndEnvWave:		.res	4
SndChannel:		.res	32
SndEnvVolCnt:		.res	4
SndEnvVolInc:		.res	4
SndEnvVolOff:		.res	4
SndEnvVolLoop:		.res	4
SndEnvVolParts:		.res	4
SndEnvVolParts2:	.res	4
SndEnvFrqCnt:		.res	4
SndEnvFrqInc:		.res	4
SndEnvFrqOff:		.res	4
SndEnvFrqLoop:		.res	4
SndEnvFrqParts:		.res	4
SndEnvFrqParts2:	.res	4
SndEnvWaveCnt:		.res	4
SndEnvWaveOff:		.res	4
SndEnvWaveLoop:		.res	4
SndEnvWaveParts:	.res	4
SndEnvWaveParts2:	.res	4

MAX_INSTRUMENTS		.set	64
SndEnvVolPtrLo:		.res	MAX_INSTRUMENTS
SndEnvVolPtrHi:		.res	MAX_INSTRUMENTS
SndEnvFrqPtrLo:		.res	MAX_INSTRUMENTS
SndEnvFrqPtrHi:		.res	MAX_INSTRUMENTS
SndEnvWavePtrLo:	.res	MAX_INSTRUMENTS
SndEnvWavePtrHi:	.res	MAX_INSTRUMENTS

	.rodata

SndOffsets:		.byte	$00,$08,$10,$18

	.code

;----------------------------------------------------------------------------
; void lynx_snd_init() will initialize the sound engine.
;

_31250Hz	.set	%101

_lynx_snd_init:
	php
	sei
	lda	#%10011000|_31250Hz
	sta	STIMCTLA
	lda	#129
	sta	STIMBKUP	; set up a 240Hz IRQ

	stz	AUD0VOL
	stz	AUD1VOL
	stz	AUD2VOL
	stz	AUD3VOL

	stz	$fd44		; all channels full volume / no attenuation
	lda	#$ff
	stz	MSTEREO

	lda	#0
	sta	AUD0CTLA
	sta	AUD1CTLA
	sta	AUD2CTLA
	sta	AUD3CTLA

	ldx	#3
	lda	#0
init0:	stz	SndActive,x
	stz	SndReqStop,x
	stz	SndEnvVol,x
	stz	SndEnvFrq,x
	stz	SndEnvWave,x
	ldy	SndOffsets,x
	sta	SndChannel+2,y
	dex
	bpl	init0
	stz	SndRetAFlag
	stz	SndRetAFlag2
	stz	SndSema
	plp
	rts

