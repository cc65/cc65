;
; Freddy Offenga & Christian Groessler, June 2004
;
; detect the DOS version we're running on
;

	.include	"atari.inc"
	.constructor	detect,26
	.export		__dos_type

; ------------------------------------------------------------------------
; DOS type detection

.segment        "INIT"

detect:	lda	#ATARIDOS
	sta	__dos_type	; set default

	lda	DOS
	cmp	#'S'		; SpartaDOS
	beq	spdos
	cmp	#'M'		; MyDOS
	beq	mydos

	ldy	#COMTAB
	lda	#$4C
	cmp	(DOSVEC),y
	bne	done

	ldy	#ZCRNAME
	cmp	(DOSVEC),y
	bne	done

	ldy	#6		; OS/A+ has a jmp here
	cmp	(DOSVEC),y
	beq	done
	lda	#OSADOS
	sta	__dos_type
	rts

spdos:	lda	#SPARTADOS
	sta	__dos_type
done:	rts

mydos:	lda	#MYDOS
	sta	__dos_type
	rts

; ------------------------------------------------------------------------
; Data

      	.bss

__dos_type:	.res	1

