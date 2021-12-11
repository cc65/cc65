;
; 2017-11-07, Piotr Fusik
;
; unsigned __fastcall__ inflatemem (unsigned char* dest,
;                                   const unsigned char* source);
;
; NOTE: Be extremely careful with modifications, because this code is heavily
; optimized for size (for example assumes certain register and flag values
; when its internal routines return). Test with the gunzip65 sample.
;

        .export         _inflatemem

        .import         incsp2
        .importzp       sp, sreg, ptr1, ptr2, ptr3, ptr4

; --------------------------------------------------------------------------
;
; Constants
;

; Argument values for getBits.
GET_1_BIT           = $81
GET_2_BITS          = $82
GET_3_BITS          = $84
GET_4_BITS          = $88
GET_5_BITS          = $90
GET_6_BITS          = $a0
GET_7_BITS          = $c0

; Huffman trees.
TREE_SIZE           = 16
PRIMARY_TREE        = 0
DISTANCE_TREE       = TREE_SIZE

; Alphabet.
LENGTH_SYMBOLS      = 1+29+2    ; EOF, 29 length symbols, two unused symbols
DISTANCE_SYMBOLS    = 30
CONTROL_SYMBOLS     = LENGTH_SYMBOLS+DISTANCE_SYMBOLS


; --------------------------------------------------------------------------
;
; Page zero
;

; Pointer to the compressed data.
inputPointer                :=  ptr1    ; 2 bytes

; Pointer to the uncompressed data.
outputPointer               :=  ptr2    ; 2 bytes

; Local variables.
; As far as there is no conflict, same memory locations are used
; for different variables.

inflateStored_pageCounter   :=  ptr3    ; 1 byte
inflateDynamic_symbol       :=  ptr3    ; 1 byte
inflateDynamic_lastLength   :=  ptr3+1  ; 1 byte
        .assert ptr4 = ptr3 + 2, error, "Need three bytes for inflateDynamic_tempCodes"
inflateDynamic_tempCodes    :=  ptr3+1  ; 3 bytes
inflateDynamic_allCodes     :=  inflateDynamic_tempCodes+1 ; 1 byte
inflateDynamic_primaryCodes :=  inflateDynamic_tempCodes+2 ; 1 byte
inflateCodes_sourcePointer  :=  ptr3    ; 2 bytes
inflateCodes_lengthMinus2   :=  ptr4    ; 1 byte
getBits_base                :=  sreg    ; 1 byte
getBit_buffer               :=  sreg+1  ; 1 byte


; --------------------------------------------------------------------------
;
; Code
;

_inflatemem:

; inputPointer = source
        sta     inputPointer
        stx     inputPointer+1
; outputPointer = dest
        ldy     #1
        lda     (sp),y
        sta     outputPointer+1
        dey
        lda     (sp),y
        sta     outputPointer

;       ldy     #0
        sty     getBit_buffer

inflate_blockLoop:
; Get a bit of EOF and two bits of block type
;       ldy     #0
        sty     getBits_base
        lda     #GET_3_BITS
        jsr     getBits
        lsr     a
; A and Z contain block type, C contains EOF flag
; Save EOF flag
        php
        bne     inflateCompressed

; Decompress a 'stored' data block.
;       ldy     #0
        sty     getBit_buffer   ; ignore bits until byte boundary
        jsr     getWord         ; skip the length we don't need
        jsr     getWord         ; get the one's complement length
        sta     inflateStored_pageCounter
        bcs     inflateStored_firstByte ; jmp
inflateStored_copyByte:
        jsr     getByte
;       sec
inflateStoreByte:
        jsr     storeByte
        bcc     inflateCodes_loop
inflateStored_firstByte:
        inx
        bne     inflateStored_copyByte
        inc     inflateStored_pageCounter
        bne     inflateStored_copyByte

; Block decompressed.
inflate_nextBlock:
        plp
        bcc     inflate_blockLoop

; Decompression complete.
; return outputPointer - dest
        lda     outputPointer
;       ldy     #0
;       sec
        sbc     (sp),y
        iny
        pha
        lda     outputPointer+1
        sbc     (sp),y
        tax
        pla
; pop dest
        jmp     incsp2

inflateCompressed:
; Decompress a Huffman-coded data block
; A=1: fixed block, initialize with fixed codes
; A=2: dynamic block, start by clearing all code lengths
; A=3: invalid compressed data, not handled in this routine
        eor     #2

;       ldy     #0
inflateCompressed_setCodeLengths:
        tax
        beq     inflateCompressed_setLiteralCodeLength
; fixed Huffman literal codes:
; 144 8-bit codes
; 112 9-bit codes
        lda     #4
        cpy     #144
        rol     a
inflateCompressed_setLiteralCodeLength:
        sta     literalSymbolCodeLength,y
        beq     inflateCompressed_setControlCodeLength
; fixed Huffman control codes:
; 24 7-bit codes
;  6 8-bit codes
;  2 meaningless 8-bit codes
; 30 5-bit distance codes
        lda     #5+DISTANCE_TREE
        cpy     #LENGTH_SYMBOLS
        bcs     inflateCompressed_setControlCodeLength
        cpy     #24
        adc     #$100+2-DISTANCE_TREE
inflateCompressed_setControlCodeLength:
        cpy     #CONTROL_SYMBOLS
        bcs     inflateCompressed_noControlSymbol
        sta     controlSymbolCodeLength,y
inflateCompressed_noControlSymbol:
        iny
        bne     inflateCompressed_setCodeLengths

        tax
        beq     inflateDynamic

; Decompress a block
inflateCodes:
        jsr     buildHuffmanTree
inflateCodes_loop:
        jsr     fetchPrimaryCode
        bcc     inflateStoreByte
        beq     inflate_nextBlock
; Copy sequence from look-behind buffer
;       ldy     #0
        sty     getBits_base
        cmp     #9
        bcc     inflateCodes_setSequenceLength
        tya
;       lda     #0
        cpx     #1+28
        bcs     inflateCodes_setSequenceLength
        dex
        txa
        lsr     a
        ror     getBits_base
        inc     getBits_base
        lsr     a
        rol     getBits_base
        jsr     getAMinus1BitsMax8
;       sec
        adc     #0
inflateCodes_setSequenceLength:
        sta     inflateCodes_lengthMinus2
        ldx     #DISTANCE_TREE
        jsr     fetchCode
        cmp     #4
        bcc     inflateCodes_setOffsetLowByte
        inc     getBits_base
        lsr     a
        jsr     getAMinus1BitsMax8
inflateCodes_setOffsetLowByte:
        eor     #$ff
        sta     inflateCodes_sourcePointer
        lda     getBits_base
        cpx     #10
        bcc     inflateCodes_setOffsetHighByte
        lda     getNPlus1Bits_mask-10,x
        jsr     getBits
        clc
inflateCodes_setOffsetHighByte:
        eor     #$ff
;       clc
        adc     outputPointer+1
        sta     inflateCodes_sourcePointer+1
        jsr     copyByte
        jsr     copyByte
inflateCodes_copyByte:
        jsr     copyByte
        dec     inflateCodes_lengthMinus2
        bne     inflateCodes_copyByte
        beq     inflateCodes_loop ; jmp

inflateDynamic:
; Decompress a block reading Huffman trees first
;       ldy     #0
; numberOfPrimaryCodes = 257 + getBits(5)
; numberOfDistanceCodes = 1 + getBits(5)
; numberOfTemporaryCodes = 4 + getBits(4)
        ldx     #3
inflateDynamic_getHeader:
        lda     inflateDynamic_headerBits-1,x
        jsr     getBits
;       sec
        adc     inflateDynamic_headerBase-1,x
        sta     inflateDynamic_tempCodes-1,x
        dex
        bne     inflateDynamic_getHeader

; Get lengths of temporary codes in the order stored in inflateDynamic_tempSymbols
;       ldx     #0
inflateDynamic_getTempCodeLengths:
        lda     #GET_3_BITS
        jsr     getBits
        ldy     inflateDynamic_tempSymbols,x
        sta     literalSymbolCodeLength,y
        ldy     #0
        inx
        cpx     inflateDynamic_tempCodes
        bcc     inflateDynamic_getTempCodeLengths

; Build the tree for temporary codes
        jsr     buildHuffmanTree

; Use temporary codes to get lengths of literal/length and distance codes
;       ldx     #0
;       sec
inflateDynamic_decodeLength:
; C=1: literal codes
; C=0: control codes
        stx     inflateDynamic_symbol
        php
; Fetch a temporary code
        jsr     fetchPrimaryCode
; Temporary code 0..15: put this length
        bpl     inflateDynamic_storeLengths
; Temporary code 16: repeat last length 3 + getBits(2) times
; Temporary code 17: put zero length 3 + getBits(3) times
; Temporary code 18: put zero length 11 + getBits(7) times
        tax
        jsr     getBits
        cpx     #GET_3_BITS
        bcc     inflateDynamic_code16
        beq     inflateDynamic_code17
;       sec
        adc     #7
inflateDynamic_code17:
;       ldy     #0
        sty     inflateDynamic_lastLength
inflateDynamic_code16:
        tay
        lda     inflateDynamic_lastLength
        iny
        iny
inflateDynamic_storeLengths:
        iny
        plp
        ldx     inflateDynamic_symbol
inflateDynamic_storeLength:
        bcc     inflateDynamic_controlSymbolCodeLength
        sta     literalSymbolCodeLength,x
        inx
        cpx     #1
inflateDynamic_storeNext:
        dey
        bne     inflateDynamic_storeLength
        sta     inflateDynamic_lastLength
        beq     inflateDynamic_decodeLength ; jmp
inflateDynamic_controlSymbolCodeLength:
        cpx     inflateDynamic_primaryCodes
        bcc     inflateDynamic_storeControl
; the code lengths we skip here were zero-initialized
; in inflateCompressed_setControlCodeLength
        bne     inflateDynamic_noStartDistanceTree
        ldx     #LENGTH_SYMBOLS
inflateDynamic_noStartDistanceTree:
        ora     #DISTANCE_TREE
inflateDynamic_storeControl:
        sta     controlSymbolCodeLength,x
        inx
        cpx     inflateDynamic_allCodes
        bcc     inflateDynamic_storeNext
        dey
;       ldy     #0
        jmp     inflateCodes

; Build Huffman trees basing on code lengths (in bits)
; stored in the *SymbolCodeLength arrays
buildHuffmanTree:
; Clear nBitCode_literalCount, nBitCode_controlCount
        tya
;       lda     #0
buildHuffmanTree_clear:
        sta     nBitCode_clearFrom,y
        iny
        bne     buildHuffmanTree_clear
; Count number of codes of each length
;       ldy     #0
buildHuffmanTree_countCodeLengths:
        ldx     literalSymbolCodeLength,y
        inc     nBitCode_literalCount,x
        bne     buildHuffmanTree_notAllLiterals
        stx     allLiteralsCodeLength
buildHuffmanTree_notAllLiterals:
        cpy     #CONTROL_SYMBOLS
        bcs     buildHuffmanTree_noControlSymbol
        ldx     controlSymbolCodeLength,y
        inc     nBitCode_controlCount,x
buildHuffmanTree_noControlSymbol:
        iny
        bne     buildHuffmanTree_countCodeLengths
; Calculate offsets of symbols sorted by code length
;       lda     #0
        ldx     #$100-4*TREE_SIZE
buildHuffmanTree_calculateOffsets:
        sta     nBitCode_literalOffset+4*TREE_SIZE-$100,x
        clc
        adc     nBitCode_literalCount+4*TREE_SIZE-$100,x
        inx
        bne     buildHuffmanTree_calculateOffsets
; Put symbols in their place in the sorted array
;       ldy     #0
buildHuffmanTree_assignCode:
        tya
        ldx     literalSymbolCodeLength,y
        ldy     nBitCode_literalOffset,x
        inc     nBitCode_literalOffset,x
        sta     codeToLiteralSymbol,y
        tay
        cpy     #CONTROL_SYMBOLS
        bcs     buildHuffmanTree_noControlSymbol2
        ldx     controlSymbolCodeLength,y
        ldy     nBitCode_controlOffset,x
        inc     nBitCode_controlOffset,x
        sta     codeToControlSymbol,y
        tay
buildHuffmanTree_noControlSymbol2:
        iny
        bne     buildHuffmanTree_assignCode
        rts

; Read Huffman code using the primary tree
fetchPrimaryCode:
        ldx     #PRIMARY_TREE
; Read a code from input using the tree specified in X.
; Return low byte of this code in A.
; Return C flag reset for literal code, set for length code.
fetchCode:
;       ldy     #0
        tya
fetchCode_nextBit:
        jsr     getBit
        rol     a
        inx
        bcs     fetchCode_ge256
; are all 256 literal codes of this length?
        cpx     allLiteralsCodeLength
        beq     fetchCode_allLiterals
; is it literal code of length X?
        sec
        sbc     nBitCode_literalCount,x
        bcs     fetchCode_notLiteral
; literal code
;       clc
        adc     nBitCode_literalOffset,x
        tax
        lda     codeToLiteralSymbol,x
fetchCode_allLiterals:
        clc
        rts
; code >= 256, must be control
fetchCode_ge256:
;       sec
        sbc     nBitCode_literalCount,x
        sec
; is it control code of length X?
fetchCode_notLiteral:
;       sec
        sbc     nBitCode_controlCount,x
        bcs     fetchCode_nextBit
; control code
;       clc
        adc     nBitCode_controlOffset,x
        tax
        lda     codeToControlSymbol,x
        and     #$1f    ; make distance symbols zero-based
        tax
;       sec
        rts

; Read A minus 1 bits, but no more than 8
getAMinus1BitsMax8:
        rol     getBits_base
        tax
        cmp     #9
        bcs     getByte
        lda     getNPlus1Bits_mask-2,x
getBits:
        jsr     getBits_loop
getBits_normalizeLoop:
        lsr     getBits_base
        ror     a
        bcc     getBits_normalizeLoop
        rts

; Read 16 bits
getWord:
        jsr     getByte
        tax
; Read 8 bits
getByte:
        lda     #$80
getBits_loop:
        jsr     getBit
        ror     a
        bcc     getBits_loop
        rts

; Read one bit, return in the C flag
getBit:
        lsr     getBit_buffer
        bne     getBit_return
        pha
;       ldy     #0
        lda     (inputPointer),y
        inc     inputPointer
        bne     getBit_samePage
        inc     inputPointer+1
getBit_samePage:
        sec
        ror     a
        sta     getBit_buffer
        pla
getBit_return:
        rts

; Copy a previously written byte
copyByte:
        ldy     outputPointer
        lda     (inflateCodes_sourcePointer),y
        ldy     #0
; Write a byte
storeByte:
;       ldy     #0
        sta     (outputPointer),y
        inc     outputPointer
        bne     storeByte_return
        inc     outputPointer+1
        inc     inflateCodes_sourcePointer+1
storeByte_return:
        rts


; --------------------------------------------------------------------------
;
; Constant data
;

        .rodata

getNPlus1Bits_mask:
        .byte   GET_1_BIT,GET_2_BITS,GET_3_BITS,GET_4_BITS,GET_5_BITS,GET_6_BITS,GET_7_BITS

inflateDynamic_tempSymbols:
        .byte   GET_2_BITS,GET_3_BITS,GET_7_BITS,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15

inflateDynamic_headerBits:
        .byte   GET_4_BITS,GET_5_BITS,GET_5_BITS
inflateDynamic_headerBase:
        .byte   3,LENGTH_SYMBOLS,0


; --------------------------------------------------------------------------
;
; Uninitialised data
;

        .bss

; Data for building trees.

literalSymbolCodeLength:
        .res    256
controlSymbolCodeLength:
        .res    CONTROL_SYMBOLS

; Huffman trees.

nBitCode_clearFrom:
nBitCode_literalCount:
        .res    2*TREE_SIZE
nBitCode_controlCount:
        .res    2*TREE_SIZE
nBitCode_literalOffset:
        .res    2*TREE_SIZE
nBitCode_controlOffset:
        .res    2*TREE_SIZE
allLiteralsCodeLength:
        .res    1

codeToLiteralSymbol:
        .res    256
codeToControlSymbol:
        .res    CONTROL_SYMBOLS
