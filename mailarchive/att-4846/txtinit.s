;
; Christian Groessler, March 2005
;
; initialize text screen (open E: device)
;
; void _txtinit (void);
;


		.import		clriocb
	 	.export		__txtinit
		.include	"atari.inc"

		.code

.proc	__txtinit

		lda	#$ff
		cmp	ICHID			; IOCB #0 open?
		beq	do_init			; no -> open it
		rts

do_init:	ldx	#0
		jsr	clriocb

		ldx	#0
		lda	#OPNOT|OPNIN
		sta	ICAX1
		lda	#<edev
		sta	ICBAL
		lda	#>edev
		sta	ICBAH
		lda	#OPEN
		sta	ICCOM

		jmp	CIOV

.endproc

		.data

edev:		.byte	"E:", ATEOL

