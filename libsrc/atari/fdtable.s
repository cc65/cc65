;
; Christian Groessler, May-2000
;
; fd indirection table & helper functions
;

	.include "atari.inc"
	.export	fdtoiocb
	.export	fdtoiocb_down
	.export	fd_table

	.data

fd_table:
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0
	.byte	0,$ff,0,0

MAX_FD_VAL	=	(* - fd_table) / 4

ft_usa  = 0	; usage counter
ft_iocb	= 1	; iocb index (0,$10,$20,etc.), $ff for empty entry
ft_dev  = 2	; device of open iocb
ft_flag = 3	; flags

	.code

; gets fd in ax, decrements usage counter
; return iocb index in X
; return N bit set for invalid fd
; return Z bit set if last user
; all registers destroyed
.proc	fdtoiocb_down

	cpx	#0
	bne	inval
	cmp	#MAX_FD_VAL
	bcs	inval
	asl	a			; create index into fd table
	asl	a
	tax
	lda	#$ff
	cmp	fd_table+ft_iocb,x	; entry in use?
	beq	inval			; no, return error
	lda	fd_table+ft_usa,x	; get usage counter
	beq	ok_notlast		; 0?
	sec
	sbc	#1			; decr usage counter
	sta	fd_table+ft_usa,x
retiocb:php
	txa
	tay
	lda	fd_table+ft_iocb,x	; get iocb
	tax
	plp
	bne	cont
	php
	lda	#$ff
	sta	fd_table+ft_iocb,y	; clear table entry
	plp
cont:	rts

ok_notlast:
	lda	#1			; clears Z
	jmp	retiocb

.endproc

inval:	ldx	#$ff			; sets N
	rts


; gets fd in ax
; return iocb index in X
; return N bit set for invalid fd
; all registers destroyed
.proc	fdtoiocb

	cpx	#0
	bne	inval
	cmp	#MAX_FD_VAL
	bcs	inval
	asl	a			; create index into fd table
	asl	a
	tax
	lda	#$ff
	cmp	fd_table+ft_iocb,x	; entry in use?
	beq	inval			; no, return error
	lda	fd_table+ft_usa,x	; get usage counter
	beq	inval			; 0? should not happen
	lda	fd_table+ft_iocb,x	; get iocb
	rts

.endproc
