;
; Kevin Ruland
;
; unsigned char __fastcall__ revers (unsigned char onoff)
;

	.export		_revers

	.include	"apple2.inc"

_revers:
	ldy	INVFLG		; Stash old value
	and	#$FF		; Test for any bit
	beq	normal		; Nothing set
	lda	#~$3F		; Not Inverse
normal:	eor	#$FF		; Xor Normal
	sta	INVFLG
	tya			; What was the old value?
	eor	#$FF		; Normal = $FF, Inverse = $3F
	beq	:+
	lda	#$01
:	ldx	#$00
	rts
