;
; Piotr Fusik, 18.11.2001
;
; unsigned __fastcall__ inflatemem (char* dest, const char* source);
;

	.export		_inflatemem

	.import		incsp2
	.importzp	sp, sreg
	.importzp	ptr1, ptr2, ptr3, ptr4
	.importzp	tmp1, tmp2, tmp3, tmp4

; --------------------------------------------------------------------------
;
; Constants
;

; Maximum length of a Huffman code
MAX_BITS      =	15
; Index in bitsCount, bitsPointer_l and bitsPointer_h for literal tree
LITERAL_TREE  =	0
; Index in bitsCount, bitsPointer_l and bitsPointer_h for distance tree
DISTANCE_TREE =	MAX_BITS
; Size of each of bitsCount, bitsPointer_l and bitsPointer_h
TREES_SIZE    =	2*MAX_BITS+1

; --------------------------------------------------------------------------
;
; Page zero
;

; Pointer to compressed data
inputPointer  =	ptr1	; 2 bytes
; Pointer to uncompressed data
outputPointer =	ptr2	; 2 bytes
; Buffer for getBit
getBitHold    =	tmp1	; 1 byte
; Local variables. Variables from different routines use same memory.
cnt           =	tmp2	; 1 byte
tmp           =	sreg	; 1 byte
ptr           =	sreg	; 2 bytes
len           =	ptr3	; 2 bytes
nl            =	tmp3	; 1 byte
nd            =	tmp4	; 1 byte
src           =	ptr4	; 2 bytes
dest          =	ptr4	; 2 bytes

; --------------------------------------------------------------------------
;
; Code
;

_inflatemem:

; inputPointer = source
	sta	inputPointer
	stx	inputPointer+1
; outputPointer = dest
	ldy	#0
	lda	(sp),y
	sta	outputPointer
	iny
	lda	(sp),y
	sta	outputPointer+1

;	ldy	#1
	sty	getBitHold
inflatemem_1:
; Get a bit of EOF and two bits of block type
	ldx	#3
	lda	#0
	jsr	getBits
	lsr	a
	tax
; X contains block type, C contains EOF flag
; Save EOF flag
	php
; Go to the routine decompressing this block
	jsr	callExtr
	plp
	bcc	inflatemem_1
; C flag is set!

; return outputPointer - dest;
	lda	outputPointer
	ldy	#0
	sbc	(sp),y	; C flag is set
	pha
	iny
	lda	outputPointer+1
	sbc	(sp),y
	tax
	pla
; pop dest
	jmp	incsp2

; --------------------------------------------------------------------------
; Go to the routine decompressing block type X

callExtr:
	lda	extr_h,x
	pha
	lda	extr_l,x
	pha
	rts

; --------------------------------------------------------------------------
; Decompress 'stored' data block

inflateCopyBlock:
; Ignore bits until byte boundary
	ldy	#1
	sty	getBitHold
; Get 16-bit length
	ldx	#inputPointer
	lda	(0,x)
	sta	len
	lda	(inputPointer),y
	sta	len+1
; Skip length and one's compliment length
	lda	#4
	clc
	adc	inputPointer
	sta	inputPointer
	bcc	moveBlock
	inc	inputPointer+1
; fall into moveBlock

; --------------------------------------------------------------------------
; Copy block of length len from (0,x) to output

moveBlock:
	ldy	len
	beq	moveBlock_1
	ldy	#0
	inc	len+1
moveBlock_1:
	lda	(0,x)
	sta	(outputPointer),y
	inc	0,x
	bne	moveBlock_2
	inc	1,x
moveBlock_2:
	inc	outputPointer
	bne	moveBlock_3
	inc	outputPointer+1
moveBlock_3:
	dec	len
	bne	moveBlock_1
	dec	len+1
	bne	moveBlock_1
	rts

; --------------------------------------------------------------------------
; Decompress Huffman-coded data block with default Huffman trees:
; literalCodeLength:  144 times 8, 112 times 9
; endCodeLength:      24 times 7, 6 times 8
; distanceCodeLength: 30 times 5, 2 times 8
;                     (2 last codes from literal tree are not used)

inflateFixedBlock:
	ldx	#159
	stx	distanceCodeLength+32
	lda	#8
inflateFixedBlock_1:
	sta	literalCodeLength-1,x
	sta	literalCodeLength+159-1,x
	dex
	bne	inflateFixedBlock_1
	ldx	#112
	lda	#9
inflateFixedBlock_2:
	sta	literalCodeLength+144-1,x
	dex
	bne	inflateFixedBlock_2
	ldx	#24
	lda	#7
inflateFixedBlock_3:
	sta	endCodeLength-1,x
	dex
	bne	inflateFixedBlock_3
	ldx	#30
	lda	#5+DISTANCE_TREE
inflateFixedBlock_4:
	sta	distanceCodeLength-1,x
	dex
	bne	inflateFixedBlock_4
	jmp	inflateCodes

; --------------------------------------------------------------------------
; Decompress Huffman-coded data block, reading Huffman trees first

inflateDynamicBlock:
; numberOfLiteralCodes = getBits(5) + 257;
	ldx	#5
	lda	#<(lengthCodeLength-1)
	jsr	getBits
	sta	nl
; numberOfDistanceCodes = getBits(5) + 1;
	ldx	#5
	lda	#1
	jsr	getBits
	sta	nd
	clc
	adc	nl
	sta	nl
; numberOfTemporaryCodes = getBits(4) + 4;
	lda	#4
	tax
	jsr	getBits
	sta	cnt
; Clear lengths of temporary codes (there're 19 temp codes max),
; clear literalCodeLength-1 (it may be used by temporary code 16)
; and leave #0 in Y
	ldy	#20
	lda	#0
inflateDynamicBlock_1:
	sta	literalCodeLength-2,y
	dey
	bne	inflateDynamicBlock_1
; Get lengths of temporary codes in order stored in bll
inflateDynamicBlock_2:
	ldx	#3
	lda	#0
	jsr	getBits	; does not change Y
	ldx	bll,y
	sta	literalCodeLength,x
	iny
	cpy	cnt
	bcc	inflateDynamicBlock_2
	ror	literalCodeLength+19	; C flag is set, so it will set b7
; Build tree for temporary codes
	jsr	buildHuffmanTree

; Use temporary codes to get lengths for literal and distance codes
; dest is target-1, so we can access last written byte by (dest,0)
	lda	#<(literalCodeLength-1)
	sta	dest
	lda	#>(literalCodeLength-1)
	sta	dest+1
inflateDynamicBlock_3:
	jsr	fetchLiteralCode
; Temporary code 0..15: put this length
	ldy	#1
	cmp	#16
	bcc	inflateDynamicBlock_6
	bne	inflateDynamicBlock_4
; Temporary code 16: repeat last length 3..6 times
	ldx	#2
	lda	#3
	jsr	getBits
	tay
	lda	(dest,x)	; X == 0
	bpl	inflateDynamicBlock_6	; branch always
inflateDynamicBlock_4:
	lsr	a
; Temporary code 17: put zero length 3..10 times
	lda	#3
	tax
	bcs	inflateDynamicBlock_5
; Temporary code 18: put zero length 11..138 times
	ldx	#7
	lda	#11
inflateDynamicBlock_5:
	jsr	getBits
	tay
	lda	#0
; Write A length Y times
inflateDynamicBlock_6:
	sty	cnt
inflateDynamicBlock_7:
	sta	(dest),y
	dey
	bne	inflateDynamicBlock_7
	lda	cnt
	clc
	adc	dest
	sta	dest
	bcc	inflateDynamicBlock_8
	inc	dest+1
inflateDynamicBlock_8:
	cmp	nl
	bne	inflateDynamicBlock_3
	ldy	dest+1
	sbc	#<endCodeLength	; C flag is set
	bcs	inflateDynamicBlock_11
	dey
inflateDynamicBlock_11:
	cpy	#>endCodeLength
	bcc	inflateDynamicBlock_3
; Mark end of distance lengths
	ldx	nd
	tay
	ror	distanceCodeLength,x	; C flag is set, so it will set b7
; Move distance lengths to distanceCodeLength table
inflateDynamicBlock_9:
	dex
	lda	endCodeLength,y
; Mark existing codes (of non-zero length) as distance tree codes
	beq	inflateDynamicBlock_10
	pha
	lda	#0
	sta	endCodeLength,y
	pla
	clc
	adc	#DISTANCE_TREE
	sta	distanceCodeLength,x
inflateDynamicBlock_10:
	dey
	txa
	bne	inflateDynamicBlock_9
; fall into inflateCodes

; --------------------------------------------------------------------------
; Decompress data block basing on given Huffman trees

inflateCodes:
	jsr	buildHuffmanTree
inflateCodes_1:
	jsr	fetchLiteralCode
	bcc	inflateCodes_2
; Literal code
	ldy	#0
	sta	(outputPointer),y
	inc	outputPointer
	bne	inflateCodes_1
	inc	outputPointer+1
	bcs	inflateCodes_1	; branch always
; End of block
inflateCodes_ret:
	rts
inflateCodes_2:
	beq	inflateCodes_ret
; Repeat block
	jsr	getValue
	sta	len
	tya
	jsr	getBits
	sta	len+1
	ldx	#DISTANCE_TREE
	jsr	fetchCode
	jsr	getValue
	sta	src
	tya
	jsr	getBits
	sta	src+1
	lda	outputPointer
	sec
	sbc	src
	sta	src
	lda	outputPointer+1
	sbc	src+1
	sta	src+1
	ldx	#src
	jsr	moveBlock
	beq	inflateCodes_1	; branch always

; --------------------------------------------------------------------------
; Build Huffman trees basing on code lengths.
; Lengths (in bits) are stored in literalCodeLength.
; A byte with highest bit set marks end of length table.

buildHuffmanTree:
	lda	#<literalCodeLength
	sta	ptr
	sta	src
	lda	#>literalCodeLength
	sta	ptr+1
	sta	src+1
; Clear counts
	ldy	#TREES_SIZE
	lda	#0
buildHuffmanTree_1:
	sta	bitsCount-1,y
	dey
	bne	buildHuffmanTree_1
	beq	buildHuffmanTree_3	; branch always
; Count number of codes of each length
buildHuffmanTree_2:
	tax
	inc	bitsCount,x
	iny
	bne	buildHuffmanTree_3
	inc	ptr+1
buildHuffmanTree_3:
	lda	(ptr),y
	bpl	buildHuffmanTree_2
; Calculate pointer for each length
	ldx	#0
	stx	bitsCount
	lda	#<sortedCodes
	ldy	#>sortedCodes
buildHuffmanTree_4:
	sta	bitsPointer_l,x
	tya
	sta	bitsPointer_h,x
	lda	bitsCount,x
	asl	a
	bcc	buildHuffmanTree_5
	iny
	clc
buildHuffmanTree_5:
	adc	bitsPointer_l,x
	bcc	buildHuffmanTree_6
	iny
buildHuffmanTree_6:
	inx
	cpx	#TREES_SIZE
	bcc	buildHuffmanTree_4
.ifpc02
	ldy	#1
.else
	ldy	#0
.endif
	bcs	buildHuffmanTree_9	; branch always
; Put codes into their place in sorted table
buildHuffmanTree_7:
	beq	buildHuffmanTree_8
	tax
	lda	bitsPointer_l,x
	sta	ptr
	clc
	adc	#2
	sta	bitsPointer_l,x
	lda	bitsPointer_h,x
	sta	ptr+1
	adc	#0
	sta	bitsPointer_h,x
	lda	src
	sbc	#<(endCodeLength-1)	; C flag is zero
.ifpc02
.else
	iny				; ldy #1
.endif
	sta	(ptr),y
	lda	src+1
	sbc	#>(endCodeLength-1)
.ifpc02
	sta	(ptr)
.else
	dey				; ldy #0
	sta	(ptr),y
.endif
buildHuffmanTree_8:
	inc	src
	bne	buildHuffmanTree_9
	inc	src+1
buildHuffmanTree_9:
.ifpc02
	lda	(src)
.else
	lda	(src),y
.endif
	bpl	buildHuffmanTree_7
	rts

; --------------------------------------------------------------------------
; Read code basing on literal tree

fetchLiteralCode:
	ldx	#LITERAL_TREE
; fall into fetchCode

; --------------------------------------------------------------------------
; Read code from input stream basing on tree given in X.
; Return code in A, C is set if non-literal code.

fetchCode:
	lda	#0
fetchCode_1:
	jsr	getBit
	rol	a
	cmp	bitsCount+1,x
	bcc	fetchCode_2
	sbc	bitsCount+1,x
	inx
	bcs	fetchCode_1	; branch always
fetchCode_2:
	ldy	bitsPointer_l,x
	sty	ptr
	ldy	bitsPointer_h,x
	asl	a
	bcc	fetchCode_3
	iny
fetchCode_3:
	sty	ptr+1
	tay
	lda	(ptr),y
	asl	a
	iny
	lda	(ptr),y
	rts

; --------------------------------------------------------------------------
; Read low byte of value (length or distance), basing on code A

getValue:
	tay
	ldx	lengthExtraBits-1,y
	lda	lengthCode_l-1,y
	pha
	lda	lengthCode_h-1,y
	tay
	pla
; fall into getBits

; --------------------------------------------------------------------------
; Read X-bit number from input stream and adds it to A.
; In case of carry, Y is incremented.
; If X > 8, only 8 bits are read.
; On return X holds number of unread bits: X = (X > 8 ? X - 8 : 0);

getBits:
	cpx	#0
	beq	getBits_ret
	pha
	lda	#1
	sta	tmp
	pla
getBits_1:
	jsr	getBit
	bcc	getBits_2
	clc
	adc	tmp
	bcc	getBits_2
	iny
getBits_2:
	dex
	beq	getBits_ret
	asl	tmp
	bcc	getBits_1
getBits_ret:
	rts

; --------------------------------------------------------------------------
; Read single bit from input stream, return it in C flag

getBit:
	lsr	getBitHold
	bne	getBit_ret
	pha
.ifpc02
	lda	(inputPointer)
.else
	tya
	pha
	ldy	#0
	lda	(inputPointer),y
.endif
	inc	inputPointer
	bne	getBit_1
	inc	inputPointer+1
getBit_1:
	ror	a	; C flag set
	sta	getBitHold
.ifpc02
.else
	pla
	tay
.endif
	pla
getBit_ret:
	rts

; --------------------------------------------------------------------------
;
; Constant data
;

	.rodata
; --------------------------------------------------------------------------
; Addresses of functions extracting different blocks
extr_l:
	.byte	<(inflateCopyBlock-1)
	.byte	<(inflateFixedBlock-1)
	.byte	<(inflateDynamicBlock-1)
extr_h:
	.byte	>(inflateCopyBlock-1)
	.byte	>(inflateFixedBlock-1)
	.byte	>(inflateDynamicBlock-1)

; --------------------------------------------------------------------------
; Order, in which lengths of temporary codes are stored
bll:
	.byte	16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15

; --------------------------------------------------------------------------
; Tables for length and distance codes
; Value is Code + getBits(ExtraBits)

; Base values
lengthCode_l:
	.byte	<3,<4,<5,<6,<7,<8,<9,<10
	.byte	<11,<13,<15,<17,<19,<23,<27,<31
	.byte	<35,<43,<51,<59,<67,<83,<99,<115
	.byte	<131,<163,<195,<227,<258
distanceCode_l:
	.byte	<1,<2,<3,<4,<5,<7,<9,<13
	.byte	<17,<25,<33,<49,<65,<97,<129,<193
	.byte	<257,<385,<513,<769,<1025,<1537,<2049,<3073
	.byte	<4097,<6145,<8193,<12289,<16385,<24577
lengthCode_h:
	.byte	>3,>4,>5,>6,>7,>8,>9,>10
	.byte	>11,>13,>15,>17,>19,>23,>27,>31
	.byte	>35,>43,>51,>59,>67,>83,>99,>115
	.byte	>131,>163,>195,>227,>258
distanceCode_h:
	.byte	>1,>2,>3,>4,>5,>7,>9,>13
	.byte	>17,>25,>33,>49,>65,>97,>129,>193
	.byte	>257,>385,>513,>769,>1025,>1537,>2049,>3073
	.byte	>4097,>6145,>8193,>12289,>16385,>24577

; Number of extra bits to read
lengthExtraBits:
	.byte	0,0,0,0,0,0,0,0
	.byte	1,1,1,1,2,2,2,2
	.byte	3,3,3,3,4,4,4,4
	.byte	5,5,5,5,0
distanceExtraBits:
	.byte	0,0,0,0,1,1,2,2
	.byte	3,3,4,4,5,5,6,6
	.byte	7,7,8,8,9,9,10,10
	.byte	11,11,12,12,13,13

; --------------------------------------------------------------------------
;
; Uninitialised data
;

	.bss

; --------------------------------------------------------------------------
; Data for building literal tree

	.res	1
; Length of literal codes
literalCodeLength:
	.res	256
; Length of 'end' code
endCodeLength:
	.res	1
; Length of 'length' codes
lengthCodeLength:
	.res	29

; --------------------------------------------------------------------------
; Data for building distance tree

distanceCodeLength:
	.res	30
; For two unused codes in fixed trees and an end flag
	.res	3

; --------------------------------------------------------------------------
; Huffman tree structure

; Number of codes of each length
bitsCount:
	.res	TREES_SIZE

; Pointer to sorted codes of each length
bitsPointer_l:
	.res	TREES_SIZE
bitsPointer_h:
	.res	TREES_SIZE

; Sorted codes
sortedCodes:
	.res	2*(256+1+29+30+2)



