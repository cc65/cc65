
	;; Keivn Ruland
	;;
	;; unsigned char wherey( void );

	.export	       	_wherey

	.include	"apple2.inc"

_wherey:
	lda	CV
        ldx     #$00
	rts
