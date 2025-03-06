    ; Taken from https://codebase64.org/doku.php?id=base:petscii_to_screencode&s[]=petscii, by Mace


    .export _petscii2scrcode
	
    
    _petscii2scrcode:
        cmp #$20		; if A<32 then...
		bcc ddRev

		cmp #$60		; if A<96 then...
		bcc dd1

		cmp #$80		; if A<128 then...
		bcc dd2

		cmp #$a0		; if A<160 then...
		bcc dd3

		cmp #$c0		; if A<192 then...
		bcc dd4

		cmp #$ff		; if A<255 then...
		bcc ddRev

		lda #$7e		; A=255, then A=126
		bne ddEnd

	dd2:	and #$5f		; if A=96..127 then strip bits 5 and 7
		bne ddEnd

	dd3:	ora #$40		; if A=128..159, then set bit 6
		bne ddEnd

	dd4:	eor #$c0		; if A=160..191 then flip bits 6 and 7
		bne ddEnd

	dd1:	and #$3f		; if A=32..95 then strip bits 6 and 7
		bpl ddEnd		; <- you could also do .byte $0c here

	ddRev:	eor #$80		; flip bit 7 (reverse on when off and vice versa)
	ddEnd:  rts
