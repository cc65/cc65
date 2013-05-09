;
; Piotr Fusik, 21.09.2003
;
; unsigned __fastcall__ inflatemem (char* dest, const char* source);
;

        .export         _inflatemem

        .import         incsp2
        .importzp       sp, sreg, ptr1, ptr2, ptr3, ptr4, tmp1

; --------------------------------------------------------------------------
;
; Constants
;

; Maximum length of a Huffman code.
MAX_BITS      = 15

; All Huffman trees are stored in the bitsCount, bitsPointer_l
; and bitsPointer_h arrays.  There may be two trees: the literal/length tree
; and the distance tree, or just one - the temporary tree.

; Index in the mentioned arrays for the beginning of the literal/length tree
; or the temporary tree.
PRIMARY_TREE  = 0

; Index in the mentioned arrays for the beginning of the distance tree.
DISTANCE_TREE = MAX_BITS

; Size of each array.
TREES_SIZE    = 2*MAX_BITS


; --------------------------------------------------------------------------
;
; Page zero
;

; Pointer to the compressed data.
inputPointer            =       ptr1    ; 2 bytes

; Pointer to the uncompressed data.
outputPointer           =       ptr2    ; 2 bytes

; Local variables.
; As far as there is no conflict, same memory locations are used
; for different variables.

inflateDynamicBlock_cnt =       ptr3    ; 1 byte
inflateCodes_src        =       ptr3    ; 2 bytes
buildHuffmanTree_src    =       ptr3    ; 2 bytes
getNextLength_last      =       ptr3    ; 1 byte
getNextLength_index     =       ptr3+1  ; 1 byte

buildHuffmanTree_ptr    =       ptr4    ; 2 bytes
fetchCode_ptr           =       ptr4    ; 2 bytes
getBits_tmp             =       ptr4    ; 1 byte

moveBlock_len           =       sreg    ; 2 bytes
inflateDynamicBlock_np  =       sreg    ; 1 byte
inflateDynamicBlock_nd  =       sreg+1  ; 1 byte

getBit_hold             =       tmp1    ; 1 byte


; --------------------------------------------------------------------------
;
; Code
;

_inflatemem:

; inputPointer = source
        sta     inputPointer
        stx     inputPointer+1
; outputPointer = dest
.ifpc02
        lda     (sp)
        ldy     #1
.else
        ldy     #0
        lda     (sp),y
        iny
.endif
        sta     outputPointer
        lda     (sp),y
        sta     outputPointer+1

;       ldy     #1
        sty     getBit_hold
inflatemem_1:
; Get a bit of EOF and two bits of block type
        ldx     #3
        lda     #0
        jsr     getBits
        lsr     a
; A and Z contain block type, C contains EOF flag
; Save EOF flag
        php
; Go to the routine decompressing this block
        jsr     callExtr
        plp
        bcc     inflatemem_1
; C flag is set!

; return outputPointer - dest;
        lda     outputPointer
.ifpc02
        sbc     (sp)            ; C flag is set
        ldy     #1
.else
        ldy     #0
        sbc     (sp),y          ; C flag is set
        iny
.endif
        pha
        lda     outputPointer+1
        sbc     (sp),y
        tax
        pla
; pop dest
        jmp     incsp2

; --------------------------------------------------------------------------
; Go to proper block decoding routine.

callExtr:
        bne     inflateCompressedBlock

; --------------------------------------------------------------------------
; Decompress a 'stored' data block.

inflateCopyBlock:
; Ignore bits until byte boundary
        ldy     #1
        sty     getBit_hold
; Get 16-bit length
        ldx     #inputPointer
        lda     (0,x)
        sta     moveBlock_len
        lda     (inputPointer),y
        sta     moveBlock_len+1
; Skip the length and one's complement of it
        lda     #4
        clc
        adc     inputPointer
        sta     inputPointer
        bcc     moveBlock
        inc     inputPointer+1
;       jmp     moveBlock

; --------------------------------------------------------------------------
; Copy block of length moveBlock_len from (0,x) to the output.

moveBlock:
        ldy     moveBlock_len
        beq     moveBlock_1
.ifpc02
.else
        ldy     #0
.endif
        inc     moveBlock_len+1
moveBlock_1:
        lda     (0,x)
.ifpc02
        sta     (outputPointer)
.else
        sta     (outputPointer),y
.endif
        inc     0,x
        bne     moveBlock_2
        inc     1,x
moveBlock_2:
        inc     outputPointer
        bne     moveBlock_3
        inc     outputPointer+1
moveBlock_3:
.ifpc02
        dey
.else
        dec     moveBlock_len
.endif
        bne     moveBlock_1
        dec     moveBlock_len+1
        bne     moveBlock_1
        rts

; --------------------------------------------------------------------------
; Decompress a Huffman-coded data block
; (A = 1: fixed, A = 2: dynamic).

inflateCompressedBlock:
        lsr     a
        bne     inflateDynamicBlock
; Note: inflateDynamicBlock may assume that A = 1

; --------------------------------------------------------------------------
; Decompress a Huffman-coded data block with default Huffman trees
; (defined by the DEFLATE format):
; literalCodeLength:  144 times 8, 112 times 9
; endCodeLength:      7
; lengthCodeLength:   23 times 7, 6 times 8
; distanceCodeLength: 30 times 5+DISTANCE_TREE, 2 times 8
;                     (two 8-bit codes from the primary tree are not used).

inflateFixedBlock:
        ldx     #159
        stx     distanceCodeLength+32
        lda     #8
inflateFixedBlock_1:
        sta     literalCodeLength-1,x
        sta     literalCodeLength+159-1,x
        dex
        bne     inflateFixedBlock_1
        ldx     #112
;       lda     #9
inflateFixedBlock_2:
        inc     literalCodeLength+144-1,x       ; sta
        dex
        bne     inflateFixedBlock_2
        ldx     #24
;       lda     #7
inflateFixedBlock_3:
        dec     endCodeLength-1,x       ; sta
        dex
        bne     inflateFixedBlock_3
        ldx     #30
        lda     #5+DISTANCE_TREE
inflateFixedBlock_4:
        sta     distanceCodeLength-1,x
        dex
        bne     inflateFixedBlock_4
        beq     inflateCodes            ; branch always

; --------------------------------------------------------------------------
; Decompress a Huffman-coded data block, reading Huffman trees first.

inflateDynamicBlock:
; numberOfPrimaryCodes = 257 + getBits(5)
        ldx     #5
;       lda     #1
        jsr     getBits
        sta     inflateDynamicBlock_np
; numberOfDistanceCodes = 1 + getBits(5)
        ldx     #5
        lda     #1+29+1
        jsr     getBits
        sta     inflateDynamicBlock_nd
; numberOfTemporaryCodes = 4 + getBits(4)
        lda     #4
        tax
        jsr     getBits
        sta     inflateDynamicBlock_cnt
; Get lengths of temporary codes in the order stored in tempCodeLengthOrder
        txa                     ; lda #0
        tay
inflateDynamicBlock_1:
        ldx     #3              ; A = 0
        jsr     getBits         ; does not change Y
inflateDynamicBlock_2:
        ldx     tempCodeLengthOrder,y
        sta     literalCodeLength,x
        lda     #0
        iny
        cpy     inflateDynamicBlock_cnt
        bcc     inflateDynamicBlock_1
        cpy     #19
        bcc     inflateDynamicBlock_2
        ror     literalCodeLength+19    ; C flag is set, so this will set b7
; Build the tree for temporary codes
        jsr     buildHuffmanTree

; Use temporary codes to get lengths of literal/length and distance codes
        ldx     #0
        ldy     #1
        stx     getNextLength_last
inflateDynamicBlock_3:
        jsr     getNextLength
        sta     literalCodeLength,x
        inx
        bne     inflateDynamicBlock_3
inflateDynamicBlock_4:
        jsr     getNextLength
inflateDynamicBlock_5:
        sta     endCodeLength,x
        inx
        cpx     inflateDynamicBlock_np
        bcc     inflateDynamicBlock_4
        lda     #0
        cpx     #1+29
        bcc     inflateDynamicBlock_5
inflateDynamicBlock_6:
        jsr     getNextLength
        cmp     #0
        beq     inflateDynamicBlock_7
        adc     #DISTANCE_TREE-1        ; C flag is set
inflateDynamicBlock_7:
        sta     endCodeLength,x
        inx
        cpx     inflateDynamicBlock_nd
        bcc     inflateDynamicBlock_6
        ror     endCodeLength,x         ; C flag is set, so this will set b7
;       jmp     inflateCodes

; --------------------------------------------------------------------------
; Decompress a data block basing on given Huffman trees.

inflateCodes:
        jsr     buildHuffmanTree
inflateCodes_1:
        jsr     fetchPrimaryCode
        bcs     inflateCodes_2
; Literal code
.ifpc02
        sta     (outputPointer)
.else
        ldy     #0
        sta     (outputPointer),y
.endif
        inc     outputPointer
        bne     inflateCodes_1
        inc     outputPointer+1
        bcc     inflateCodes_1  ; branch always
; End of block
inflateCodes_ret:
        rts
inflateCodes_2:
        beq     inflateCodes_ret
; Restore a block from the look-behind buffer
        jsr     getValue
        sta     moveBlock_len
        tya
        jsr     getBits
        sta     moveBlock_len+1
        ldx     #DISTANCE_TREE
        jsr     fetchCode
        jsr     getValue
        sec
        eor     #$ff
        adc     outputPointer
        sta     inflateCodes_src
        php
        tya
        jsr     getBits
        plp
        eor     #$ff
        adc     outputPointer+1
        sta     inflateCodes_src+1
        ldx     #inflateCodes_src
        jsr     moveBlock
        beq     inflateCodes_1  ; branch always

; --------------------------------------------------------------------------
; Build Huffman trees basing on code lengths (in bits).
; stored in the *CodeLength arrays.
; A byte with its highest bit set marks the end.

buildHuffmanTree:
        lda     #<literalCodeLength
        sta     buildHuffmanTree_src
        lda     #>literalCodeLength
        sta     buildHuffmanTree_src+1
; Clear bitsCount and bitsPointer_l
        ldy     #2*TREES_SIZE+1
        lda     #0
buildHuffmanTree_1:
        sta     bitsCount-1,y
        dey
        bne     buildHuffmanTree_1
        beq     buildHuffmanTree_3      ; branch always
; Count number of codes of each length
buildHuffmanTree_2:
        tax
        inc     bitsPointer_l,x
        iny
        bne     buildHuffmanTree_3
        inc     buildHuffmanTree_src+1
buildHuffmanTree_3:
        lda     (buildHuffmanTree_src),y
        bpl     buildHuffmanTree_2
; Calculate a pointer for each length
        ldx     #0
        lda     #<sortedCodes
        ldy     #>sortedCodes
        clc
buildHuffmanTree_4:
        sta     bitsPointer_l,x
        tya
        sta     bitsPointer_h,x
        lda     bitsPointer_l+1,x
        adc     bitsPointer_l,x         ; C flag is zero
        bcc     buildHuffmanTree_5
        iny
buildHuffmanTree_5:
        inx
        cpx     #TREES_SIZE
        bcc     buildHuffmanTree_4
        lda     #>literalCodeLength
        sta     buildHuffmanTree_src+1
        ldy     #0
        bcs     buildHuffmanTree_9      ; branch always
; Put codes into their place in sorted table
buildHuffmanTree_6:
        beq     buildHuffmanTree_7
        tax
        lda     bitsPointer_l-1,x
        sta     buildHuffmanTree_ptr
        lda     bitsPointer_h-1,x
        sta     buildHuffmanTree_ptr+1
        tya
        ldy     bitsCount-1,x
        inc     bitsCount-1,x
        sta     (buildHuffmanTree_ptr),y
        tay
buildHuffmanTree_7:
        iny
        bne     buildHuffmanTree_9
        inc     buildHuffmanTree_src+1
        ldx     #MAX_BITS-1
buildHuffmanTree_8:
        lda     bitsCount,x
        sta     literalCount,x
        dex
        bpl     buildHuffmanTree_8
buildHuffmanTree_9:
        lda     (buildHuffmanTree_src),y
        bpl     buildHuffmanTree_6
        rts

; --------------------------------------------------------------------------
; Decode next code length using temporary codes.

getNextLength:
        stx     getNextLength_index
        dey
        bne     getNextLength_1
; Fetch a temporary code
        jsr     fetchPrimaryCode
; Temporary code 0..15: put this length
        ldy     #1
        cmp     #16
        bcc     getNextLength_2
; Temporary code 16: repeat last length 3 + getBits(2) times
; Temporary code 17: put zero length 3 + getBits(3) times
; Temporary code 18: put zero length 11 + getBits(7) times
        tay
        ldx     tempExtraBits-16,y
        lda     tempBaseValue-16,y
        jsr     getBits
        cpy     #17
        tay
        txa                     ; lda #0
        bcs     getNextLength_2
getNextLength_1:
        lda     getNextLength_last
getNextLength_2:
        sta     getNextLength_last
        ldx     getNextLength_index
        rts

; --------------------------------------------------------------------------
; Read a code basing on the primary tree.

fetchPrimaryCode:
        ldx     #PRIMARY_TREE
;       jmp     fetchCode

; --------------------------------------------------------------------------
; Read a code from input basing on the tree specified in X.
; Return low byte of this code in A.
; For the literal/length tree, the C flag is set if the code is non-literal.

fetchCode:
        lda     #0
fetchCode_1:
        jsr     getBit
        rol     a
        inx
        sec
        sbc     bitsCount-1,x
        bcs     fetchCode_1
        adc     bitsCount-1,x   ; C flag is zero
        cmp     literalCount-1,x
        sta     fetchCode_ptr
        ldy     bitsPointer_l-1,x
        lda     bitsPointer_h-1,x
        sta     fetchCode_ptr+1
        lda     (fetchCode_ptr),y
        rts

; --------------------------------------------------------------------------
; Decode low byte of a value (length or distance), basing on the code in A.
; The result is the base value for this code plus some bits read from input.

getValue:
        tay
        ldx     lengthExtraBits-1,y
        lda     lengthBaseValue_l-1,y
        pha
        lda     lengthBaseValue_h-1,y
        tay
        pla
;       jmp     getBits

; --------------------------------------------------------------------------
; Read X-bit number from the input and add it to A.
; Increment Y if overflow.
; If X > 8, read only 8 bits.
; On return X holds number of unread bits: X = (X > 8 ? X - 8 : 0);

getBits:
        cpx     #0
        beq     getBits_ret
.ifpc02
        stz     getBits_tmp
        dec     getBits_tmp
.else
        pha
        lda     #$ff
        sta     getBits_tmp
        pla
.endif
getBits_1:
        jsr     getBit
        bcc     getBits_2
        sbc     getBits_tmp     ; C flag is set
        bcc     getBits_2
        iny
getBits_2:
        dex
        beq     getBits_ret
        asl     getBits_tmp
        bmi     getBits_1
getBits_ret:
        rts

; --------------------------------------------------------------------------
; Read a single bit from input, return it in the C flag.

getBit:
        lsr     getBit_hold
        bne     getBit_ret
        pha
.ifpc02
        lda     (inputPointer)
.else
        sty     getBit_hold
        ldy     #0
        lda     (inputPointer),y
        ldy     getBit_hold
.endif
        inc     inputPointer
        bne     getBit_1
        inc     inputPointer+1
getBit_1:
        ror     a       ; C flag is set
        sta     getBit_hold
        pla
getBit_ret:
        rts


; --------------------------------------------------------------------------
;
; Constant data
;

        .rodata
; --------------------------------------------------------------------------
; Arrays for the temporary codes.

; Order, in which lengths of the temporary codes are stored.
tempCodeLengthOrder:
        .byte   16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15

; Base values.
tempBaseValue:
        .byte   3,3,11

; Number of extra bits to read.
tempExtraBits:
        .byte   2,3,7

; --------------------------------------------------------------------------
; Arrays for the length and distance codes.

; Base values.
lengthBaseValue_l:
        .byte   <3,<4,<5,<6,<7,<8,<9,<10
        .byte   <11,<13,<15,<17,<19,<23,<27,<31
        .byte   <35,<43,<51,<59,<67,<83,<99,<115
        .byte   <131,<163,<195,<227,<258
distanceBaseValue_l:
        .byte   <1,<2,<3,<4,<5,<7,<9,<13
        .byte   <17,<25,<33,<49,<65,<97,<129,<193
        .byte   <257,<385,<513,<769,<1025,<1537,<2049,<3073
        .byte   <4097,<6145,<8193,<12289,<16385,<24577
lengthBaseValue_h:
        .byte   >3,>4,>5,>6,>7,>8,>9,>10
        .byte   >11,>13,>15,>17,>19,>23,>27,>31
        .byte   >35,>43,>51,>59,>67,>83,>99,>115
        .byte   >131,>163,>195,>227,>258
distanceBaseValue_h:
        .byte   >1,>2,>3,>4,>5,>7,>9,>13
        .byte   >17,>25,>33,>49,>65,>97,>129,>193
        .byte   >257,>385,>513,>769,>1025,>1537,>2049,>3073
        .byte   >4097,>6145,>8193,>12289,>16385,>24577

; Number of extra bits to read.
lengthExtraBits:
        .byte   0,0,0,0,0,0,0,0
        .byte   1,1,1,1,2,2,2,2
        .byte   3,3,3,3,4,4,4,4
        .byte   5,5,5,5,0
distanceExtraBits:
        .byte   0,0,0,0,1,1,2,2
        .byte   3,3,4,4,5,5,6,6
        .byte   7,7,8,8,9,9,10,10
        .byte   11,11,12,12,13,13


; --------------------------------------------------------------------------
;
; Uninitialised data
;

        .bss

; Number of literal codes of each length in the primary tree
; (MAX_BITS bytes, overlap with literalCodeLength).
literalCount:

; --------------------------------------------------------------------------
; Data for building the primary tree.

; Lengths of literal codes.
literalCodeLength:
        .res    256
; Length of the end code.
endCodeLength:
        .res    1
; Lengths of length codes.
lengthCodeLength:
        .res    29

; --------------------------------------------------------------------------
; Data for building the distance tree.

; Lengths of distance codes.
distanceCodeLength:
        .res    30
; For two unused codes in the fixed trees and an 'end' mark.
        .res    3

; --------------------------------------------------------------------------
; The Huffman trees.

; Number of codes of each length.
bitsCount:
        .res    TREES_SIZE
; Pointers to sorted codes of each length.
bitsPointer_l:
        .res    TREES_SIZE+1
bitsPointer_h:
        .res    TREES_SIZE

; Sorted codes.
sortedCodes:
        .res    256+1+29+30+2



