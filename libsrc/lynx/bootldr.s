;
; Karri Kaksonen, 2011
;
; This bootloader creates a signed binary so that the Lynx will accept it.
;
        .include "lynx.inc"
        .include "extzp.inc"
        .import         __BLOCKSIZE__
        .export         __BOOTLDR__: absolute = 1


; ------------------------------------------------------------------------
; Bootloader

        .segment "BOOTLDR"
;**********************************
; Here is the bootloader in plaintext
; The idea is to make the smalles possible encrypted loader as decryption
; is very slow. The minimum size is 49 bytes plus a zero byte.
;**********************************
;       EXE = $fb68
;
;       .org $0200
;
;       ; 1. force Mikey to be in memory
;       stz MAPCTL
;
;       ; 3. set ComLynx to open collector
;       lda #4          ; a = 00000100
;       sta SERCTL      ; set the ComLynx to open collector
;
;       ; 4. make sure the ROM is powered on
;       lda #8          ; a = 00001000
;       sta IODAT       ; set the ROM power to on
;
;       ; 5. read in secondary exe + 8 bytes from the cart and store it in $f000
;       ldx #0          ; x = 0
;       ldy #$97        ; y = secondary loader size (151 bytes)
;rloop1: lda RCART0     ; read a byte from the cart
;       sta EXE,X       ; EXE[X] = a
;       inx             ; x++
;       dey             ; y--
;       bne rloop1      ; loops until y wraps
;
;       ; 6. jump to secondary loader
;       jmp EXE         ; run the secondary loader
;
;       .reloc
;**********************************
; After compilation, encryption and obfuscation it turns into this.
;**********************************
        .byte $ff, $81, $ca, $33, $be, $80, $a2, $c4 
        .byte $6d, $98, $fe, $8d, $bc, $66, $c0, $7a 
        .byte $09, $50, $23, $28, $18, $c8, $06, $70 
        .byte $58, $4f, $1b, $e1, $c7, $90, $08, $cd 
        .byte $1a, $6e, $5a, $45, $32, $d7, $6d, $c6 
        .byte $8a, $e5, $d8, $5c, $a0, $e8, $4f, $7a 
        .byte $5f, $73, $8d, $22

;**********************************
; Now we have the secondary loader
;**********************************
        .org $fb68
        ; 1. Read in the 1st File-entry (main exe) in FileEntry
        ldx #$00
        ldy #8
rloop:  lda RCART0      ; read a byte from the cart
        sta _FileEntry,X ; EXE[X] = a
        inx
        dey
        bne rloop

        ; 2. Set the block hardware to the main exe start
        lda     _FileStartBlock
        sta     _FileCurrBlock
        jsr     seclynxblock

        ; 3. Skip over the block offset
        lda     _FileBlockOffset+1
        eor     #$FF
        tay
        lda     _FileBlockOffset
        eor     #$FF
        tax
        jsr     seclynxskip0

        ; 4. Read in the main exe to RAM
        lda     _FileDestAddr
        ldx     _FileDestAddr+1
        sta     _FileDestPtr
        stx     _FileDestPtr+1
        lda     _FileFileLen+1
        eor     #$FF
        tay
        lda     _FileFileLen
        eor     #$FF
        tax
        jsr     seclynxread0

        ; 5. Jump to start of the main exe code
        jmp     (_FileDestAddr)

;**********************************
; Skip bytes on bank 0
; X:Y count (EOR $FFFF)
;**********************************
seclynxskip0:
        inx
        bne @0
        iny
        beq exit
@0:     jsr secreadbyte0
        bra seclynxskip0

;**********************************
; Read bytes from bank 0
; X:Y count (EOR $ffff)
;**********************************
seclynxread0:
        inx
        bne @1
        iny
        beq exit
@1:     jsr secreadbyte0
        sta (_FileDestPtr)
        inc _FileDestPtr
        bne seclynxread0
        inc _FileDestPtr+1
        bra seclynxread0

;**********************************
; Read one byte from cartridge
;**********************************
secreadbyte0:
        lda RCART0
        inc _FileBlockByte
        bne exit
        inc _FileBlockByte+1
        bne exit

;**********************************
; Select a block 
;**********************************
seclynxblock:
        pha
        phx
        phy
        lda __iodat
        and #$fc
        tay
        ora #2
        tax
        lda _FileCurrBlock
        inc _FileCurrBlock
        sec
        bra @2
@0:     bcc @1
        stx IODAT
        clc
@1:     inx
        stx SYSCTL1
        dex
@2:     stx SYSCTL1
        rol
        sty IODAT
        bne @0
        lda __iodat
        sta IODAT
        stz _FileBlockByte
        lda #<($100-(>__BLOCKSIZE__))
        sta _FileBlockByte+1
        ply
        plx
        pla

exit:   rts

        .reloc

