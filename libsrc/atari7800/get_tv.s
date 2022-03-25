;
; Karri Kaksonen, 2022-03-25
;
; unsigned char get_tv (void)
;
	.include	"atari7800.inc"
	.export		_get_tv

.segment	"DATA"

_paldetected:
	.byte	$FF

; ---------------------------------------------------------------
; unsigned char get_tv (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_get_tv: near

.segment	"CODE"

	ldx     #$00
	lda     #$FF
	cmp     _paldetected
	bne     L8
L1:	lda     $0028
	and     #$80
	bne     L1
L2:	lda     $0028
	and     #$80
	beq     L2
L3:	lda     $0028
	and     #$80
	bne     L3
	lda     #$00
	sta     M0001
	jmp     L5
L4:	sta     $0024
	sta     $0024
	dec     M0001
L5:	lda     $0028
	and     #$80
	beq     L4
	lda     M0001
	cmp     #$78
	bcc     L6
	lda     #$00
	jmp     L7
L6:	lda     #$01
L7:	sta     _paldetected
	ldx     #$00
L8:	lda     _paldetected
	rts

.segment	"BSS"

M0001:
	.res	1,$00

.endproc

