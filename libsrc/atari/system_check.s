;
; Atari XL startup system check
;
; This routine gets loaded prior to the main part of the executable
; and checks if the system is compatible to run the program.
; It checks whether the system is an XL type one and that enough
; memory is installed (which isn't the case for a 600XL).
; If the system doesn't qualify, the loading of the main program
; is aborted by jumping to DOSVEC.
;
; Christian Groessler, chris@groessler.org, 2013
;

DEBUG	=	1

.if .defined(__ATARIXL__)

	.export		syschk
        .import         __SYSCHK_LOAD__
        .import         __SAVEAREA_LOAD__

        .include        "zeropage.inc"
        .include        "atari.inc"


.macro print_string text
	.local	start, cont
	jmp	cont
start:	.byte	text, ATEOL
cont:	ldx	#0		; channel 0
	lda	#<start
	sta	ICBAL,x		; address
	lda	#>start
	sta	ICBAH,x
	lda	#<(cont - start)
	sta	ICBLL,x		; length
	lda	#>(cont - start)
	sta	ICBLH,x
	lda	#PUTCHR
	sta	ICCOM,x
	jsr	CIOV_org
.endmacro
.macro print_string2 addr, len

	ldx	#0		; channel 0
	lda	#<addr
	sta	ICBAL,x		; address
	lda	#>addr
	sta	ICBAH,x
	lda	#<len
	sta	ICBLL,x		; length
	lda	#>len
	sta	ICBLH,x
	lda	#PUTCHR
	sta	ICCOM,x
	jsr	CIOV_org

.endmacro


; ------------------------------------------------------------------------
; Chunk header

.segment        "SYSCHKHDR"

        .word   __SYSCHK_LOAD__
        .word   trailer - 1

; ------------------------------------------------------------------------
; Actual code

.segment        "SYSCHK"

syschk:
	lda     $fcd8		; from ostype.s
        cmp     #$a2
        bne     is_xl

; no XL machine
	print_string "This program needs an XL machine."
	jmp	fail

; we have an XL machine, now check memory
is_xl:	lda	RAMSIZ
	cmp	#$80
	bcs	sys_ok

; not enough memory
	print_string "Not enough memory."
fail:	jsr	delay
	jmp	(DOSVEC)

lowadr:	.res	2		; lowest address we need in order to move screen memory down, depending on start address of program


; system is basically supported, check if there is enough space btw. MEMLO and our start address
; to move screen memory there

CMPVAL = 64+255+992		; you may ask, why these values...   @@@ document

sys_ok:	lda	#<__SAVEAREA_LOAD__
	sec
	sbc	#<CMPVAL
	sta	lowadr
	lda	#>__SAVEAREA_LOAD__
	sbc	#>CMPVAL
	sta	lowadr+1

	sec
	lda	MEMLO
	sbc	lowadr
	lda	MEMLO+1
	sbc	lowadr+1
	bcc	memlo_ok

; load address was too low
	print_string2 lmemerr_txt, lmemerr_txt_len
	jsr	delay		; long text takes longer to read, give user additional time
	jmp	fail

; all is well(tm), launch the application
memlo_ok:
.ifdef DEBUG
	print_string "Stage #1 OK"
	jsr	delay
.endif
	rts


lmemerr_txt:
	.byte	"Not enough memory to move screen", ATEOL
	.byte	"memory to low memory. Consider using", ATEOL
	.byte	"a higher load address.", ATEOL
lmemerr_txt_len	= * - lmemerr_txt


; short delay
.proc	delay

	lda	#10
l:	jsr	delay1
	clc
	sbc	#0
	bne	l
	rts

delay1:	ldx	#0
	ldy	#0
loop:	dey
	bne	loop
	dex
	bne	loop
	rts

.endproc

; ------------------------------------------------------------------------
; Chunk "trailer" - sets INITAD

trailer:
        .word   INITAD
        .word   INITAD+1
        .word   __SYSCHK_LOAD__

.endif	; .if .defined(__ATARIXL__)
