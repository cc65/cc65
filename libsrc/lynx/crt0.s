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
; This must be the *first* file on the linker command line
;

	.include        "lynx.inc"
	.export         _exit
	.exportzp       __iodat,__iodir,__viddma,__sprsys

	.import         initlib, donelib
	.import         zerobss
	.import	     	callmain
	.import         _main
	.import         __BSS_LOAD__
	.import         __RAM_START__, __RAM_SIZE__

	.include        "zeropage.inc"


; ------------------------------------------------------------------------
; EXE header (BLL header)

	.segment "EXEHDR"
	.word   $0880
	.dbyt   __RAM_START__
       	.dbyt   __BSS_LOAD__ - 1
	.byte   $42,$53
	.byte   $39,$33


; ------------------------------------------------------------------------
; Mikey and Suzy init data, reg offsets and data

	.rodata

SuzyInitReg:    .byte $28,$2a,$04,$06,$92,$83,$90
SuzyInitData:   .byte $7f,$7f,$00,$00,$24,$f3,$01

MikeyInitReg:   .byte $00,$01,$08,$09,$20,$28,$30,$38,$44,$50,$8a,$8b,$8c,$92,$93
MikeyInitData:  .byte $9e,$18,$68,$1f,$00,$00,$00,$00,$00,$ff,$1a,$1b,$04,$0d,$29


; ------------------------------------------------------------------------
; mikey and suzy shadow registers

       .segment "EXTZP" : zeropage

__iodat:    .res    1
__iodir:    .res    1
__viddma:   .res    1
__sprsys:   .res    1


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

	lda     #<(__RAM_START__ + __RAM_SIZE__)
	sta     sp
	lda     #>(__RAM_START__ + __RAM_SIZE__)
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

; Call module constructors

	jsr     initlib

; Push arguments and call main

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; Endless loop

noret:  bra     noret


