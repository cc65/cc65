;
; Christian Groessler, August 2000
;
; this file provides the lseek() function
;
; off_t __fastcall__ lseek(int fd, off_t offset, int whence);
;


 	.export		_lseek
	.import		incsp6,__errno,__oserror
	.importzp	sreg
	.include	"atari.inc"
	.include	"../common/errno.inc"

.proc	_lseek

; dummy implementation, return -1 and ENOSYS errno value
	jsr	incsp6
	lda	#<ENOSYS
	sta	__errno
	lda	#>ENOSYS
	sta	__errno
	ldx	#0
	stx	__oserror
	dex
	txa
	sta	sreg
	sta	sreg+1
	rts

.endproc
