; ***
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; ***
;
; Startup code for cc65 (Lynx version).  Based on Atari 8-bit startup
; code structure.  The C stack is located at the end of the RAM memory
; segment and grows downward.  Bastian Schick's executable header is put
; on the front of the fully linked binary (see EXEHDR segment.)
;

	.include        "lynx.inc"
	.export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

	.import         callirq, initlib, donelib
	.import         zerobss
	.import	     	callmain
	.import         _main
	.import         __BSS_LOAD__
	.import	       	__INTERRUPTOR_COUNT__
	.import         __RAM_START__, __RAM_SIZE__, __STACKSIZE__
	.import         __BSS_SIZE__

	.include        "zeropage.inc"
        .include        "extzp.inc"


; ------------------------------------------------------------------------
; EXE header
__BLOCKSIZE__=1024
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

__LOADER_SIZE__=410

	; The cart starts with an encrypted loader for 1024 bytes/block
	; images. The size of the loader is 410 bytes followed by two
	; mandatory directory entries.
	.byte	$FD,$C1,$0D,$8E,$E9,$EE,$09,$13,$E5,$96
	.byte	$0C,$34,$64,$DA,$D4,$BB,$99,$EC,$CE,$4F
	.byte	$AA,$8C,$ED,$65,$F0,$32,$70,$A3,$84,$C4
	.byte	$FC,$A2,$6D,$3A,$F8,$77,$4B,$AC,$9B,$54
	.byte	$7D,$82,$6F,$F8,$A5,$06,$4D,$7B,$77,$55
	.byte	$E4,$31,$C4,$2C,$2F,$2F,$B6,$4D,$15,$A9
	.byte	$C7,$99,$5D,$6E,$B3,$97,$92,$44,$7B,$2B
	.byte	$85,$18,$E6,$F1,$96,$F4,$C4,$DE,$A4,$CF
	.byte	$79,$E2,$C1,$1A,$E0,$0C,$93,$C5,$26,$BD
	.byte	$A3,$16,$8A,$C3,$59,$A0,$39,$38,$A0,$3B
	.byte	$EF,$BB,$1D,$5C,$0D,$1D,$CC,$48,$1D,$DD
	.byte	$98,$9A,$7A,$F7,$96,$F9,$61,$03,$50,$DA
	.byte	$47,$69,$94,$C3,$80,$DA,$A9,$99,$A1,$21
	.byte	$2B,$2E,$7D,$F5,$E4,$F7,$B3,$5C,$A8,$14
	.byte	$FA,$E9,$06,$AC,$1E,$9F,$B5,$31,$BE,$42
	.byte	$14,$08,$0E,$05,$FB,$25,$BB,$5C,$5C,$66
	.byte	$76,$8E,$36,$E8,$EB,$39,$F2,$26,$BD,$17
	.byte	$29,$F4,$B8,$1D,$7E,$EE,$47,$61,$BB,$9E
	.byte	$F5,$72,$C9,$BC,$26,$37,$D5,$78,$8F,$D0
	.byte	$CE,$95,$21,$EB,$4A,$07,$8D,$3A,$3A,$01
	.byte	$82,$CF,$01,$C5,$1E,$1D,$A8,$41,$4F,$BD
	.byte	$C1,$76,$22,$A3,$88,$D9,$57,$C9,$51,$3A
	.byte	$26,$BE,$4A,$1A,$7F,$42,$61,$CF,$FC,$FC
	.byte	$5B,$06,$94,$D2,$2C,$78,$45,$BA,$93,$C4
	.byte	$7D,$7C,$81,$73,$07,$4F,$E2,$6C,$E9,$81
	.byte	$1A,$DE,$77,$74,$87,$DE,$26,$9E,$7A,$A8
	.byte	$19,$A7,$34,$32,$70,$ED,$59,$A8,$4A,$D8
	.byte	$FE,$CB,$DD,$02,$2F,$CE,$92,$E9,$13,$A6
	.byte	$FF,$B4,$4B,$18,$9D,$63,$48,$E0,$3B,$3B
	.byte	$0D,$2B,$FC,$04,$A4,$E3,$5E,$4C,$3C,$94
	.byte	$70,$C4,$F0,$64,$15,$48,$68,$17,$DE,$14
	.byte	$72,$F0,$59,$33,$4C,$49,$47,$8D,$B6,$F4
	.byte	$82,$4E,$B7,$4E,$01,$C9,$C2,$82,$0B,$7A
	.byte	$AC,$67,$9B,$0F,$04,$E1,$B6,$78,$34,$C8
	.byte	$4F,$2A,$11,$ED,$D0,$1C,$6D,$CD,$3D,$47
	.byte	$09,$8B,$E5,$38,$19,$7A,$31,$6E,$30,$71
	.byte	$1C,$90,$34,$E5,$44,$CC,$00,$C7,$41,$D0
	.byte	$27,$8A,$06,$29,$5C,$2B,$E4,$26,$63,$09
	.byte	$52,$D3,$97,$33,$D7,$59,$1C,$36,$2F,$C9
	.byte	$A9,$A2,$B5,$BB,$A9,$1D,$E6,$36,$7E,$56
	.byte	$05,$A4,$9C,$E0,$45,$59,$21,$E1,$E6,$21

	; The directory structure required by the 410 byte bootloader
__DIRECTORY_START__:
; Entry 0 - title sprite (mandatory)
off0=__LOADER_SIZE__+(__DIRECTORY_END__-__DIRECTORY_START__)
blocka=off0/__BLOCKSIZE__
len0=(__TITLE_END__-__TITLE_START__)
        .byte   <blocka
        .word   off0 & (__BLOCKSIZE__ - 1)
        .byte   $00
        .word   __TITLE_START__
        .word   len0

; Entry 1 - first executable (mandatory)
off1=off0+len0
block1=off1/__BLOCKSIZE__
len1=__RAM_SIZE__-__BSS_SIZE__
        .byte   <block1
        .word   off1 & (__BLOCKSIZE__ - 1)
        .byte   $88
        .word   __RAM_START__
        .word   len1
__DIRECTORY_END__:
	.org $b000
__TITLE_START__:
	; The palette for the title sprite. Complete black.
	.byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
	; The title sprite
cls_sprite:
        .byte   %00000001			; A pixel sprite
       	.byte   %00010000
       	.byte   %00100000
       	.addr   0,pixel_bitmap
       	.word   0
       	.word   0
       	.word   $a000  	                        ; 160
       	.word   $6600	                        ; 102
       	.byte   $00
pixel_bitmap:
        .byte   3,%10000100,%00000000, $0       ; A pixel bitmap
__TITLE_END__:
	.reloc

; ------------------------------------------------------------------------
; Mikey and Suzy init data, reg offsets and data

	.rodata

SuzyInitReg:    .byte $28,$2a,$04,$06,$92,$83,$90
SuzyInitData:   .byte $7f,$7f,$00,$00,$24,$f3,$01

MikeyInitReg:   .byte $00,$01,$08,$09,$20,$28,$30,$38,$44,$50,$8a,$8b,$8c,$92,$93
MikeyInitData:  .byte $9e,$18,$68,$1f,$00,$00,$00,$00,$00,$ff,$1a,$1b,$04,$0d,$29


; ------------------------------------------------------------------------
; Actual code

	.segment "STARTUP"

; set up system

	sei
	cld
	ldx     #$FF
	txs

; init bank switching

       	lda     #$C
       	sta     MAPCTL                  ; $FFF9

; disable all timer interrupts

       	lda     #$80
       	trb     TIM0CTLA
       	trb     TIM1CTLA
       	trb     TIM2CTLA
       	trb     TIM3CTLA
       	trb     TIM5CTLA
       	trb     TIM6CTLA
       	trb     TIM7CTLA

; disable TX/RX IRQ, set to 8E1

       	lda     #%11101
       	sta     SERCTL

; clear all pending interrupts

       	lda     INTSET
       	sta     INTRST

; setup the stack

	lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
	sta     sp
	lda     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
	sta     sp+1

; Init Mickey

	ldx     #.sizeof(MikeyInitReg)-1
mloop:  ldy     MikeyInitReg,x
	lda     MikeyInitData,x
	sta     $fd00,y
	dex
	bpl     mloop

; these are RAM-shadows of read only regs

	ldx     #$1b
	stx     __iodat
	dex                             ; $1A
       	stx     __iodir
	ldx     #$d
	stx     __viddma

; Init Suzy

	ldx     #.sizeof(SuzyInitReg)-1
sloop:  ldy     SuzyInitReg,x
	lda     SuzyInitData,x
	sta     $fc00,y
	dex
	bpl     sloop

	lda     #$24
	sta     __sprsys

; Clear the BSS data

	jsr     zerobss

; If we have IRQ functions, set the IRQ vector
; as Lynx is a console there is not much point in releasing the IRQ

        lda     #<__INTERRUPTOR_COUNT__
      	beq	NoIRQ1
      	lda	#<IRQStub
      	ldx	#>IRQStub
      	sei
      	sta	INTVECTL
      	stx	INTVECTH
      	cli

; Call module constructors

NoIRQ1: jsr     initlib

; Push arguments and call main

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; Endless loop

noret:  bra     noret


	.segment "CODE"
IRQStub:
	phy
	phx
	pha
	cld
       	jsr    	callirq
	lda	INTSET
	sta	INTRST
	pla
	plx
	ply
	rti

