
	;; Keivn Ruland
	;;
	;; unsigned char wherex( void );
	;; unsigned char wherey( void );

	.export		_wherex, _wherey

	.include	"apple2.inc"

_wherex:
	lda	CH
	rts

_wherey:
	lda	CV
	rts
	