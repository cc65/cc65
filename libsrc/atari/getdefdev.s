;
; Freddy Offenga & Christian Groessler, August 2003
;
; function to get default device: char *getdefdev(void);
;
; SpartaDOS:
; the ZCRNAME routine is only used to get the default drive because
; ZCRNAME has two disadvantages:
; 1. It will convert D: into D1: instead of Dn: (n = default drive)
; 2. It will give a 'no arguments' status if it detects something
;    like Dn: (without filename).
;
; OS/A+ DOS:
; ZCRNAME is slightly different from SpartaDOS. It will convert D:
; into Dn: where n is the default drive.

	.include	"atari.inc"
	.import		__dos_type
	.export		_getdefdev		; get default device (e.g. "D1:")

; Get default device (LBUF will be destroyed!!)

_getdefdev:

	lda	__dos_type	; which DOS?
	cmp	#ATARIDOS
	beq	finish

	ldy	#BUFOFF
	lda	#0
	sta	(DOSVEC),y	; reset buffer offset

; Store dummy argument

	ldy	#LBUF
	lda	#'X'
	sta	(DOSVEC),y
	iny
	lda	#ATEOL
	sta	(DOSVEC),y

; One extra store to avoid the buggy sequence from OS/A+ DOS:
; <D><RETURN><:> => drive number = <RETURN>

	iny
	sta	(DOSVEC),y

; Create crunch vector

	ldy	#ZCRNAME+1
	lda	(DOSVEC),y
	sta	crvec+1
	iny
	lda	(DOSVEC),y
	sta	crvec+2

crvec:	jsr	$FFFF		; will be set to crunch vector

; Get default device

	ldy	#COMFNAM	;  COMFNAM is always "Dn:"
	lda	(DOSVEC),y
	sta	defdev
	iny
	lda	(DOSVEC),y
	sta	defdev+1

; return ointer to default device

finish:	lda	#<defdev
	ldx	#>defdev
	rts

	.data

; Default device

defdev:
	.byte	"D1:", 0

