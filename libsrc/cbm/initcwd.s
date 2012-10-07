;
; Stefan Haubenthal, 2005-06-08
;
; CBM _curunit to _cwd
;

	.export		initcwd
	.import		__curunit, __cwd, devicestr
	.importzp	ptr2

	.segment	"INIT"

.proc	initcwd

	lda	#<__cwd
	ldx	#>__cwd
	sta	ptr2
	stx	ptr2+1
	lda	__curunit
	jmp	devicestr

.endproc
