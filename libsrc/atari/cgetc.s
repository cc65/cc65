;
; get a kbd char.
;
; char cgetc(void)
;

        .include "atari.inc"
        .export _cgetc
        
_cgetc:
	jsr	@1
	ldx	#0
	rts

@1:	lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha
	lda	#12
	sta	ICAX1Z		; fix problems with direct call to KEYBDV
        rts
