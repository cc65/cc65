;
; this routine updates errno.  do a JMP here right after calling
; CIOV.	 we expect status in Y.
; __retminus is a routine with returns AX with $FFFF
;
	.include "errno.inc"

	.export __do_oserror, __retminus

__do_oserror:
	sty	__oserror	; save os dependent error code
__retminus:
	lda	#$FF
	tax			; return -1
	rts
