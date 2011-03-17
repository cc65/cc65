;
; Karri Kaksonen, 2011
;
; This header is required for cart builds.
;
	.include "lynx.inc"
	.include "extzp.inc"
	.import         __RAM_START__
	.import         __CODE_SIZE__,__DATA_SIZE__,__RODATA_SIZE__
	.import		__STARTUP_SIZE__,__INIT_SIZE__
	.import		__BLOCKSIZE__
	.export		__EXEHDR__: absolute = 1


; ------------------------------------------------------------------------
; EXE header
	.segment "EXEHDR"
	.byte	'L','Y','N','X'				; magic
	.word	__BLOCKSIZE__				; bank 0 page size
	.word	__BLOCKSIZE__				; bank 1 page size
	.word	1					; version number
	.asciiz	"Cart name                      "	; 32 bytes cart name
	.asciiz	"Manufacturer   "			; 16 bytes manufacturer
	.byte	0					; rotation 1=left
							; rotation 2=right
	.byte	0,0,0,0,0				; spare

;**********************************
; Here is the bootloader in plaintext
; The idea is to make the smalles possible encrypted loader as decryption
; is very slow. The minimum size is 49 bytes plus a zero byte.
;**********************************
;	EXE = $f000
;
;	.org $0200
;
;	; 1. force Mikey to be in memory
;	stz	MAPCTL
;
;	; 3. set ComLynx to open collector
;	lda #4          ; a = 00000100
;	sta SERCTL      ; set the ComLynx to open collector
;
;	; 4. make sure the ROM is powered on
;	lda #8          ; a = 00001000
;	sta IODAT       ; set the ROM power to on
;
;	; 5. read in secondary exe + 8 bytes from the cart and store it in $f000
;	ldx #0          ; x = 0
;	ldy #$AB        ; y = secondary loader
;rloop1: lda RCART0     ; read a byte from the cart
;	sta EXE,X       ; EXE[X] = a
;	inx             ; x++
;	dey             ; y--
;	bne rloop1      ; loops until y wraps
;
;	; 6. jump to secondary loader
;	jmp EXE         ; run the secondary loader
;
;	.reloc
;**********************************
; After compilation, encryption and obfuscation it turns into this.
;**********************************
__LOADER_SIZE__=52
	.byte $ff, $dc, $e3, $bd, $bc, $7f, $f8, $94 
	.byte $b7, $dd, $68, $bb, $da, $5b, $50, $5c 
	.byte $ea, $9f, $2b, $df, $96, $80, $3f, $7e 
	.byte $ef, $15, $81, $ae, $ad, $e4, $6e, $b3 
	.byte $46, $d7, $72, $58, $f7, $76, $8a, $4a 
	.byte $c7, $99, $bd, $ff, $02, $3e, $5b, $3f 
	.byte $0c, $49, $1b, $22

;**********************************
; Now we have the secondary loader
;**********************************
__LOADER2_SIZE__=171
	.org $f000
	; 1. Read in the 1st File-entry (main exe) in FileEntry
	ldx #$00
	ldy #8
rloop:	lda RCART0      ; read a byte from the cart
	sta _FileEntry,X ; EXE[X] = a
	inx
	dey
	bne rloop

	; 2. Set the block hardware to the main exe start
	lda	_FileStartBlock
	sta	_FileCurrBlock
	jsr	seclynxblock

	; 3. Skip over the block offset
	lda	_FileBlockOffset
	ldx	_FileBlockOffset+1
	phx				; The BLL kit uses negative offsets
	plx				; while the basic Lynx uses positive
	bmi	@1			; Make all offsets negative
        eor	#$FF
	pha
	txa
        eor	#$FF
	bra	@2
@1:	pha
	txa
@2:	tay
	plx
	jsr	seclynxskip0

	; 4. Read in the main exe to RAM
	lda	_FileDestAddr
	ldx	_FileDestAddr+1
	sta     _FileDestPtr
	stx     _FileDestPtr+1
	lda     _FileFileLen
	ldx     _FileFileLen+1
	phx			; The BLL kit uses negative counts
	plx			; while the basic Lynx uses positive
	bmi	@3		; make all counts negative
	eor	#$FF
	pha
	txa
	eor	#$FF
	bra	@4
@3:	pha
	txa
@4:	tay
	plx
	jsr     seclynxread0

	; 5. Jump to start of the main exe code
	jmp	(_FileDestAddr)

;**********************************
; Skip bytes on bank 0
; X:Y count (EOR $FFFF)
;**********************************
seclynxskip0:
	inx
	bne @0
	iny
	beq exit
@0:	jsr secreadbyte0
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
@1:	jsr secreadbyte0
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
@0:	bcc @1
	stx IODAT
	clc
@1:	inx
	stx SYSCTL1
	dex
@2:	stx SYSCTL1
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

exit:	rts
	.reloc

__DIRECTORY_START__:
; Entry 0 - title sprite (mandatory)
off0=__LOADER_SIZE__+__LOADER2_SIZE__+(__DIRECTORY_END__-__DIRECTORY_START__)
blocka=off0/__BLOCKSIZE__
; Entry 0 - first executable
block0=off0/__BLOCKSIZE__
len0=__STARTUP_SIZE__+__INIT_SIZE__+__CODE_SIZE__+__DATA_SIZE__+__RODATA_SIZE__
        .byte   <block0
        .word   off0 & (__BLOCKSIZE__ - 1)
        .byte   $88
        .word   __RAM_START__
        .word   len0
__DIRECTORY_END__:

