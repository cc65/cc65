	;;
	;; Kevin Ruland
	;;
	;; unsigned char __fastcall__ revers (unsigned char onoff)
	;;

	.export		_revers

	.include	"apple2.inc"

_revers:
	ldy	TEXTTYP		; Stash old value
	and	#$FF		; Test for any bit
	bne	reverse		; Nothing set
	lda	#$FF
reverse:	
	ora	#$3F	
	sta	TEXTTYP
	tya			; What was the old value?
	eor	#$FF		; Normal = $FF, Reverse = $3F
	beq	L2
	lda	#01
L2:	
	rts

