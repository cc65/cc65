;
; Freddy Offenga & Christian Groessler, December 2004
;
; function to get default device: char *_getdefdev(void);
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
	.export		__getdefdev		; get default device (e.g. "D1:")
.ifdef	DYNAMIC_DD
	.export		__defdev
.endif

; Get default device (LBUF will be destroyed!!)

__getdefdev:

.ifdef	DEFAULT_DEVICE
	lda	#'0'+DEFAULT_DEVICE
	sta	__defdev+1
.endif
	lda	__dos_type	; which DOS?
	cmp	#ATARIDOS
	beq	finish
	cmp	#MYDOS
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
	sta	__defdev
	iny
	lda	(DOSVEC),y
	sta	__defdev+1

; Return pointer to default device

finish:	lda	#<__defdev
	ldx	#>__defdev
	rts

	.data

; Default device

__defdev:
	.byte	"D1:", 0

