;
; get a kbd char.
;
; char cgetc(void)
;

        .include "atari.inc"
        .export _cgetc
        
_cgetc:
        lda     KEYBDV+5
        pha
        lda     KEYBDV+4
        pha
        rts
	ldx	#0
	rts
