; ... change system memory variables bla


	jmp	cont

lowadr:	.word	__SAVEAREA_LOAD__ & $FF00		; our load address, rounded down to page boundary
tstadr:	.res	2
tstadr2:.res	2
tmp:	.res	1


; When setting a display mode, the ROM takes the RAMTOP value
; and subtracts the size of the screen memory from it. This will
; become the new screen memory address.
; From this address it subtracts the size of the display list.
; This will become the new display list address.
; Screen memory cannot cross 4K boundaries and a display list
; cannot cross a 1K boundary.
; Work out a sane value for RAMTOP to prevent boundary crossing.
; RAMTOP is only one byte, it counts in memory pages.

cont:

_SCRBUFSZ =	(40 * 24)		; size of mode 0 screen buffer
_DLSZ	=	32			; size of mode 0 display list

; subtract screen memory size from our load address

	lda	lowadr
	sec
	sbc	#<_SCRBUFSZ
	sta	tstadr
	lda	lowadr+1
	sbc	#>_SCRBUFSZ
	sta	tstadr+1

; check if a 4K boundary is crossed

	lda	lowadr+1
	and	#$f0
	sta	tmp
	lda	tstadr+1
	and	#$f0
	cmp	tmp
	beq	scrmemok

; 4K boundary will be crossed, take 4K boundary address as lowadr

al4k:	lda	lowadr+1
	and	#$f0
	tax
	dex
	stx	lowadr+1
	bne	cont

; subtract display list size from calculated screen address

scrmemok:
	lda	tstadr
	sec
	sbc	#<_DLSZ
	sta	tstadr2
	lda	tstadr+1
	sbc	#>_DLSZ
	sta	tstadr2+1

; check if a 1K boundary is crossed

	lda	tstadr+1
	and	#$fc
	sta	tmp
	lda	tstadr2+1
	and	#$fc
	cmp	tmp
	bne	al4k		; 1K boundary will be crossed, decrease lowadr

; address of display list is ok

dlok:

; decrease tstadr2 by two

	lda	tstadr2
	sec
	sbc	#2
	sta	tstadr2
	bcs	dec_cont
	lda	tstadr2+1
	sbc	#0
	sta	tstadr2+1
dec_cont:
