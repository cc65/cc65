;
; Christian Groessler, May-2000
;
; fd indirection table & helper functions
;

	.include "atari.inc"
	.importzp tmp2,ptr4,sp
	.import	subysp,addysp
	.export	fdtoiocb
	.export	fdtoiocb_down
	.export	fd_table
	.export	fddecusage
	.export	newfd

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
		; lower 3 bits: device number (for R: and D:)

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

; decrements usage counter for fd
; if 0 reached, it's marked as unused
; get fd index in tmp2
; Y register preserved
.proc	fddecusage

	lda	tmp2			; get fd
	cmp	#MAX_FD_VAL
	bcs	ret			; invalid index, do nothing
	asl	a			; create index into fd table
	asl	a
	tax
	lda	#$ff
	cmp	fd_table+ft_iocb,x	; entry in use?
	beq	ret			; no, do nothing
	lda	fd_table+ft_usa,x	; get usage counter
	beq	ret			; 0? should not happen
	sec
	sbc	#1			; decrement by one
	sta	fd_table+ft_usa,x
	bne	ret			; not 0
	lda	#$ff			; 0, table entry unused now
	sta	fd_table+ft_iocb,x	; clear table entry
ret:	rts

.endproc

; newfd
;
; called from open() function
; finds a fd to use for an open request
; checks whether it's a device or file (file: characters following the ':')
; files always get an exclusive slot
; for devices it is checked whether the device is already open, and if yes,
; a link to this open device is returned
;
; Calling paramteter:
;	AX   - points to filename
;	Y    - iocb to use (if we need a new open)
; Return parameters:
;	tmp2 - fd num
;	C    - 0/1 for no open needed/open should be performed
; all registers preserved!

; local variables:
;   AX     - 0 (A-0,X-1)
;   Y      - 2
;   ptr4   - 3,4  (backup)
;   devnum - 5

loc_A      = 0
loc_X      = 1
loc_Y      = 2
loc_ptr4_l = 3
loc_ptr4_h = 4
loc_devnum = 5
loc_size   = 6

.proc	newfd

	pha
	txa
	pha
	tya
	pha

	ldy	#loc_size
	jsr	subysp
	ldy	#loc_devnum
	lda	#0
	sta	(sp),y		; loc_devnum
	dey
	lda	ptr4+1
	sta	(sp),y		; loc_ptr4_h
	dey
	lda	ptr4
	sta	(sp),y		; loc_ptr4_l
	dey
	pla
	sta	(sp),y		; loc_Y
	dey
	pla
	sta	(sp),y		; loc_X
	sta	ptr4+1
	dey
	pla
	sta	(sp),y		; loc_A
	sta	ptr4

	; ptr4 points to filename

	ldy	#1
	lda	#':'
	cmp	(ptr4),y	; "X:"
	beq	colon1
	iny
	cmp	(ptr4),y	; "Xn:"
	beq	colon2

	; no colon there!? OK, then we use a fresh iocb....

do_open:nop			; @@@TODO
	ldy	#loc_size
	jsr	addysp
	rts

colon2:	dey
	tya
	pha
	lda	(ptr4),y	; get device number
	sec
	sbc	#'0'
	ldy	#loc_devnum
	sta	(sp),y		; save it
	pla
	tay
colon1:

.endproc

