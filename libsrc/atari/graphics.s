;
; Christian Groessler, December 2000
;
; this file provides an equivalent to the BASIC GRAPHICS function
;
; void __fastcall__ graphics(unsigned char mode);

	.export	_graphics
	.constructor	scr_memtop,28

	.import	__graphmode_used

	.include	"atari.inc"

	.code


.proc	_graphics

	rts		; not implemented yet!

.endproc


; calc. upper memory limit to use

.proc	scr_memtop

	lda	__graphmode_used
	beq	ignore		; mode 0 doesn't need adjustment
	cmp	#32
	bcs	ignore		; invalid value

	asl	a
	tay
	lda	APPMHI
	sec
	sbc	grmemusage,y
	sta	APPMHI
	lda	APPMHI+1
	sbc	grmemusage+1,y
	sta	APPMHI+1
ignore:	rts

.endproc

	.rodata

; memory usage of the different graphics modes (0-31)
; values < 0 of "bytes needed" are mappped to 0
;               bytes needed    ; mode	; val. of MEMTOP
grmemusage:
	.word	0		; 0	; 39967
	.word	0 ;-318		; 1	; 40285
	.word	0 ;-568		; 2	; 40535
	.word	0 ;-558		; 3	; 40525
	.word	0 ;-298		; 4	; 40265
	.word	182		; 5	; 39785
	.word	1182		; 6	; 38785
	.word	3198		; 7	; 36769
	.word	7120		; 8	; 32847
	.word	7146		; 9	; 32821
	.word	7146		; 10	; 32821
	.word	7146		; 11	; 32821
	.word	162		; 12	; 39805
	.word	0 ;-328		; 13	; 40295
	.word	3278		; 14	; 36689
	.word	7120		; 15	; 32847
	.word	0		; 16	; 39967
	.word	0 ;-320		; 17	; 40287
	.word	0 ;-572		; 18	; 40539
	.word	0 ;-560		; 19	; 40527
	.word	0 ;-296		; 20	; 40263
	.word	184		; 21	; 39783
	.word	1192		; 22	; 38775
	.word	3208		; 23	; 36759
	.word	7146		; 24	; 32821
	.word	7146		; 25	; 32821
	.word	7146		; 26	; 32821
	.word	7146		; 27	; 32821
	.word	160		; 28	; 39807
	.word	0 ;-332		; 29	; 40299
	.word	3304		; 30	; 36663
	.word	7146		; 31	; 32821

; the program used to get these values (Atari BASIC):
;  100 FILE=0
;  110 IF FILE=1 THEN OPEN #1,8,0,"D:FREEMEM.OUT"
;  120 IF FILE<>1 THEN OPEN #1,8,0,"E:"
;  200 DIM G(32)
;  210 FOR I=0 TO 32:GRAPHICS I:GOSUB 1000:G(I)=VAL:NEXT I
;  220 GRAPHICS 0
;  230 FOR I=0 TO 31:PRINT #1;I;":",G(I);" - ";G(0)-G(I):NEXT I
;  240 CLOSE #1
;  999 END
;  1000 VAL=PEEK(741)+256*PEEK(742)
;  1010 RETURN

