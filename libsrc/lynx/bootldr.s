;
; Karri Kaksonen, 2011
;
; This bootloader creates a signed binary so that the Lynx will accept it.
;
        .include "lynx.inc"
        .include "extzp.inc"
        .import         __BANK0BLOCKSIZE__
        .export         __BOOTLDR__: absolute = 1


; ------------------------------------------------------------------------
; Bootloader

        .segment "BOOTLDR"
;**********************************
; Here is the bootloader in plaintext
; The idea is to make the smallest possible encrypted loader as decryption
; is very slow. The minimum size is 49 bytes plus a zero byte.
;**********************************
;                  EXE = $fb68
;
;                  .org $0200
;
;                  ; 1. force Mikey to be in memory
; 9C F9 FF         stz MAPCTL
;
;                  ; 2. clear palette
; A0 1F            ldy #31
; A9 00            lda #0
; 99 A0 FD nextc:  sta GCOLMAP, y
; 88               dey
; 10 FA            bpl nextc
;
;                  ; 3. set ComLynx to open collector
; A9 04            lda #4          ; a = 00000100
; 8D 8C FD         sta SERCTL      ; set the ComLynx to open collector
;
;                  ; 4. set AUDIN to output
; A9 1A            lda #$1a        ; audin = out, rest = out,
;                                  ; noexp = in, cart addr = out, ext pwd = in
; 8D 8A FD         sta IODIR
;
;                  ; 5. set AUDIN to LOW
; A9 0B            lda #$0B        ; Set AUDIN low
; 85 1A            sta $1a         ; Save local copy to ZP
; 8D 8B FD         sta IODAT
;
;                  ; 6. read in secondary exe + 8 bytes from the cart
;                  ; and store it in $f000
; A2 00            ldx #0          ; x = 0
; A0 97            ldy #$97        ; y = secondary loader size (151 bytes)
; AD B2 FC rloop1: lda RCART0      ; read a byte from the cart
; 9D 68 FB         sta EXE,X       ; EXE[X] = a
; E8               inx             ; x++
; 88               dey             ; y--
; D0 F6            bne rloop1      ; loops until y wraps
;
;                  ; 7. jump to secondary loader
; 4C 68 FB         jmp EXE
; 00 00 00 00      ; spares
; 00               ; End of encrypted header mark
;
;   .reloc
;**********************************
; After compilation, encryption and obfuscation it turns into this.
;**********************************
        .byte $ff, $b6, $bb, $82, $d5, $9f, $48, $cf
        .byte $23, $37, $8e, $07, $38, $f5, $b6, $30
        .byte $d6, $2f, $12, $29, $9f, $43, $5b, $2e
        .byte $f5, $66, $5c, $db, $93, $1a, $78, $55
        .byte $5e, $c9, $0d, $72, $1b, $e9, $d8, $4d
        .byte $2f, $e4, $95, $c0, $4f, $7f, $1b, $66
        .byte $8b, $a7, $fc, $21

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
        lda #<($100-(>__BANK0BLOCKSIZE__))
        sta _FileBlockByte+1
        ply
        plx
        pla

exit:   rts

        .reloc

