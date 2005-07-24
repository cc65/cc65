;
; Stefan Haubenthal, 2005-06-08
;
; CBM _curunit to _cwd
;

	.export		initcwd
	.import		__curunit, __cwd
	.import		pusha0, tosudiva0
	.importzp	sreg, ptr1

	.macpack	generic

	.segment	"INIT"

.proc	initcwd

	lda	__curunit
	jsr	pusha0
	lda	#10
	jsr	tosudiva0
	ldx	#0
	lda	sreg
	beq	:+		; >=10
	add	#'0'
	sta	__cwd
	inx
:	lda	ptr1		; rem
	add	#'0'
	sta	__cwd,x
	lda	#0
	sta	__cwd+1,x
	rts

.endproc
