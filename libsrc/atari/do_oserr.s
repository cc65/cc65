;
; __do_oserror updates __oserror.  Do a JMP here right after calling
; CIOV.	 It will return with AX set to -1 ($FFFF).  It expects the CIO
; status in Y.
;
; __retminus is a routine whitch returns with AX set to -1 ($FFFF).
;
	.include "errno.inc"

	.export __do_oserror, __retminus

__do_oserror:
	sty	__oserror	; save os dependent error code
__retminus:
	lda	#$FF
	tax			; return -1
	rts
