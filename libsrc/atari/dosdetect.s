;
; Freddy Offenga & Christian Groessler, August 2003
;
; detect the DOS version we're running on
;

	.include	"atari.inc"
	.constructor	detect,26
	.export		__dos_type

; DOS type detection

detect:	lda	#ATARIDOS
	sta	__dos_type	; set default

	lda	DOS
	cmp	#$53		; "S" (SpartaDOS)
	beq	spdos

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
	bne	done

spdos:	lda	#SPARTADOS
	sta	__dos_type
done:	rts

	.bss

__dos_type:	.res	1

