;
; Christian Groessler, July 2000
; from Freddy Offenga's rominfo.c
;
; unsigned int get_ostype(void)
;
; x x x x x x x x  x x x x x x x x   -	16 bit flag
; | | | | | | | |  | | | | | | | |
; | | | | | | | |  | | | | | +-+-+-- main OS rev.
; | | | | | | | |  | | | +-+-------- PAL/NTSC flag
; | | | | | | | |  +-+-+------------ minor OS rev.
; +-+-+-+-+-+-+-+------------------- unused
;
; main OS rev.:
;	000 - unknown
;	001 - 400/800 ROM
;	010 - 1200XL ROM
;	011 - XL/XE ROM
;	1xx - unassigned
; PAL/NTSC flag:
;	00 - unknown
;	01 - PAL
;	10 - NTSC
;	11 - (invalid)
; minor OS rev.: (depending on main OS rev.);
;	400/800:
;		000 - unknown
;		001 - Rev. A
;		010 - Rev. B
;		011 - unassigned (up to 111)
;	1200XL:
;		000 - unknown
;		001 - Rev. 10
;		010 - Rev. 11
;		011 - unassigned (up to 111)
;	XL/XE:
;		000 - unknown
;		001 - Rev. 1
;		010 - Rev. 2
;		011 - Rev. 3
;		100 - Rev. 4
;		101 - unassigned (up to 111)
;


	.include	"atari.inc"
	.export		_get_ostype

.proc	_get_ostype

	lda	$fcd8
	cmp	#$a2
	beq	_400800
	lda	$fff1
	cmp	#1
	beq	_1200xl
	lda	$fff7
	cmp	#1
	bcc	_unknown
	cmp	#5
	bcs	_unknown

;XL/XE ROM

	sec
	asl	a
	asl	a
	asl	a
	asl	a
	asl	a
	ora	#%11
_fin:	ldx	#0
	rts

; unknown ROM

_unknown:
	lda	#0
	tax
	rts

; 1200XL ROM

_1200xl:
	lda	$fff7		; revision #
	cmp	#10
	beq	_1200_10
	cmp	#11
	beq	_1200_11
	lda	#0		; for unknown
	beq	_1200_fin

_1200_10:
	lda	#%00100000
	bne	_1200_fin

_1200_11:
	lda	#%01000000

_1200_fin:
	ora	#%010
	bne	_fin

; 400/800 ROM

_400800:
	lda	$fff8
	ldx	$fff9
	cmp	#$dd
	bne	_400800_1
	cpx	#$57
	bne	_400800_unknown

; 400/800 NTSC Rev. A

	lda	#%00110001
	bne	_400800_done

; 400/900 unknown

_400800_unknown:
	lda	#%00000001
	bne	_400800_done

_400800_1:
	cmp	#$d6
	bne	_400800_2
	cpx	#$57
	bne	_400800_unknown

; 400/800 PAL Rev. A

	lda	#%00101001
	bne	_400800_done

_400800_2:
	cmp	#$f3
	bne	_400800_3
	cpx	#$e6
	bne	_400800_unknown

; 400/800 NTSC Rev. B

	lda	#%01010001
	bne	_400800_done

_400800_3:

	cmp	#$22
	bne	_400800_unknown
	cpx	#$58
	bne	_400800_unknown

; 400/800 PAL Rev. B

	lda	#%01001001

_400800_done:
	bne	_fin

.endproc
