
	;; Keivn Ruland
	;;
	;; unsigned char wherex( void );

	.export	       	_wherex

	.include	"apple2.inc"

.proc   _wherex

	lda    	CH
        ldx     #0
	rts

.endproc

