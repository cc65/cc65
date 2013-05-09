;
; Ullrich von Bassewitz, 07.08.1998
;
; unsigned DbgDisAsm (char* buf, unsigned addr);
; unsigned DbgDisAsm (unsigned addr);
;
;
; Part of this code is taken from the Plus/4 machine language monitor
; (TEDMon).
;

        .import         popax
        .import         __hextab, OffsetTab, AdrFlagTab
        .import         SymbolTab1, SymbolTab2, MnemoTab1, MnemoTab2



; -------------------------------------------------------------------------
; Equates for better readability

        .importzp       sreg, tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3

BufIndex        = tmp1          ; Index into output buffer
OperandLen      = tmp2          ; Length of operand
BufLen          = tmp3          ; Length of output buffer
AdrFlagBuf      = tmp4          ; Flag for addressing mode
YSave           = sreg          ; Temp storage
XSave           = sreg+1        ; Dito
BufPtr          = ptr1          ; Pointer to output buffer
MemPtr          = ptr2          ; Pointer to memory to disassemble
MnemoBuf        = ptr3          ; Buffer for decoding mnemonic


; -------------------------------------------------------------------------
; Main entries

        .export         _DbgDisAsm, _DbgDisAsmLen

.proc   _DbgDisAsm
        sta     BufLen          ; Save the buffer length
        jsr     popax           ; Get the buffer pointer
        sta     BufPtr
        stx     BufPtr+1
        jsr     popax           ; Get the address
        sta     MemPtr
        stx     MemPtr+1
        lda     #0
        sta     BufIndex        ; Initialize index into buffer
        jsr     DisAssLine      ; Disassemble one line into the buffer

        lda     BufLen          ; Get requested length
        sec
        sbc     BufIndex
        beq     L2
        tax                     ; Count into X
        ldy     BufIndex
        lda     #$20            ; Get a space
L1:     sta     (BufPtr),y
        iny
        dex
        bne     L1
L2:     lda     #0              ; Add C string terminator
        sta     (BufPtr),y
        beq     disassret

.endproc


_DbgDisAsmLen:
        sta     MemPtr          ; Save address
        stx     MemPtr+1
        ldy     #$00
        lda     (MemPtr),y      ; Get the opcode from memory...
        jsr     AnalyzeOPCode   ; ...and analyze it
disassret:
        ldx     OperandLen      ; Get length of operand
        inx                     ; Adjust for opcode byte
        txa
        ldx     #$00            ; Clear high byte
        rts     

; -------------------------------------------------------------------------
; Helper functions


Put3Spaces:
        jsr     PutSpace
Put2Spaces:
        jsr     PutSpace
PutSpace:
        lda     #$20
PutChar:
        sty     YSave           ; Save Y
        ldy     BufIndex        ; Get current line pointer
        cpy     BufLen          ; Be sure not to overflow the buffer
        bcs     PC9
        sta     (BufPtr),y      ; store character
        iny                     ; bump index
        sty     BufIndex
PC9:    ldy     YSave           ; get old value
        rts

; Print the 16 bit hex value in X/Y

PutHex16:
        txa
        jsr     PutHex8
        tya

; Print 8 bit value in A, save X and Y

PutHex8:
        stx     XSave
        sty     YSave
        ldy     BufIndex
        pha
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        tax
        lda     __hextab,x
        sta     (BufPtr),y
        iny
        pla
        and     #$0F
        tax
        lda     __hextab,x
        sta     (BufPtr),y
        iny
        sty     BufIndex
        ldy     YSave
        ldx     XSave
        rts

; -------------------------------------------------------------------------
; Disassemble one line

DisAssLine:
        ldy     MemPtr
        ldx     MemPtr+1
        jsr     PutHex16                ; Print the address
        jsr     Put2Spaces              ; Add some space
        ldy     #$00
        lda     (MemPtr),y              ; Get the opcode from memory...
        jsr     AnalyzeOPCode           ; ...and analyze it
        pha                             ; Save mnemonic
        ldx     OperandLen              ; Number of bytes

; Print the bytes that make up the instruction

        inx
L2083:  dex
        bpl     L208C                   ; Print the instruction bytes
        jsr     Put3Spaces              ; If none left, print spaces instead
        jmp     L2094
L208C:  lda     (MemPtr),y              ; Get a byte from memory
        jsr     PutHex8                 ; ...and print it
        jsr     PutSpace                ; Add some space

L2094:  iny                             ; Next one...
        cpy     #$03                    ; Maximum is three
        bcc     L2083                   ;

        jsr     Put2Spaces              ; Add some space after bytes

; Print the assembler mnemonic

        pla                             ; Get mnemonic code
        ldx     #$03
        jsr     PutMnemo                ; Print the mnemonic
        ldx     #$06

; Print the operand

L20A4:  cpx     #$03
        bne     L20BA
        ldy     OperandLen
        beq     L20BA

L20AC:  lda     AdrFlagBuf
        cmp     #$E8                    ; Branch?
        lda     (MemPtr),y              ; Get branch offset
        bcs     GetBranchAdr            ; If branch: Calculate address
        jsr     PutHex8                 ; Otherwise print 8bit value
        dey
        bne     L20AC

L20BA:  asl     AdrFlagBuf
        bcc     L20CC
        lda     SymbolTab1-1,x
        jsr     PutChar
        lda     SymbolTab2-1,x
        beq     L20CC
        jsr     PutChar

L20CC:  dex
        bne     L20A4
        rts

; If the instruction is a branch, calculate the absolute address of the
; branch target and print it.

GetBranchAdr:
        jsr     L20DD
        clc
        adc     #$01
        bne     L20D9
        inx                             ; Bump high byte
L20D9:  tay
        jmp     PutHex16                ; Output address

L20DD:  ldx     MemPtr+1
        tay
        bpl     L20E3
        dex
L20E3:  adc     MemPtr
        bcc     L20E8
        inx                             ; Bump high byte
L20E8:  rts

; -------------------------------------------------------------------------
; Subroutine to analyze an opcode byte in A. Will return a byte that
; encodes the mnemonic, and will set the number of bytes needed for this
; instruction in OperandLen

AnalyzeOPCode:
        tay
        lsr     a
        bcc     L20F8
        lsr     a
        bcs     L2107
        cmp     #$22
        beq     L2107
        and     #$07
        ora     #$80
L20F8:  lsr     a
        tax
        lda     OffsetTab,x
        bcs     L2103
        lsr     a
        lsr     a
        lsr     a
        lsr     a
L2103:  and     #$0F
        bne     L210B
L2107:  ldy     #$80
        lda     #$00
L210B:  tax
        lda     AdrFlagTab,x
        sta     AdrFlagBuf
        and     #$03
        sta     OperandLen
        tya
        and     #$8F
        tax
        tya
        ldy     #$03
        cpx     #$8A
        beq     L212B

L2120:  lsr     a
        bcc     L212B
        lsr     a
L2124:  lsr     a
        ora     #$20
        dey
        bne     L2124
        iny
L212B:  dey
        bne     L2120
        rts

; -------------------------------------------------------------------------
; Print the mnemonic with code in A (that code was returned by
; AnalyzeOpcode).

PutMnemo:
        tay
        lda     MnemoTab1,y
        sta     MnemoBuf
        lda     MnemoTab2,y
        sta     MnemoBuf+1
L213A:  lda     #$00
        ldy     #$05            ; 3*5 bits in two bytes
L213E:  asl     MnemoBuf+1
        rol     MnemoBuf
        rol     a
        dey
        bne     L213E
        adc     #$3F
        jsr     PutChar
        dex
        bne     L213A
        jmp     PutSpace

