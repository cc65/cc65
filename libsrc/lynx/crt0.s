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
; Startup code for cc65 (Lynx version).  Based on the Atari 8-bit startup
; code structure.  The C stack is located at the end of the RAM memory
; segment, and grows downward.  Bastian Schick's executable header is put
; on the front of the fully linked binary (see EXEHDR segment.)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss
        .import         callmain
        .import         _main
        .import         __MAIN_START__, __MAIN_SIZE__, __STACKSIZE__

        .include        "zeropage.inc"
        .include        "extzp.inc"
        .include        "lynx.inc"

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

; Set up the system.

        sei
        ldx     #$FF
        txs

; Init the bank switching.

        lda     #$C
        sta     MAPCTL          ; $FFF9

; Disable all timer interrupts.

        lda     #$80
        trb     TIM0CTLA
        trb     TIM1CTLA
        trb     TIM2CTLA
        trb     TIM3CTLA
        trb     TIM5CTLA
        trb     TIM6CTLA
        trb     TIM7CTLA

; Disable the TX/RX IRQ; set to 8E1.

        lda     #PAREN|RESETERR|TXOPEN|PAREVEN  ; #%00011101
        sta     SERCTL

; Clear all pending interrupts.

        lda     INTSET
        sta     INTRST

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1

; Init Mickey.

        ldx     #.sizeof(MikeyInitReg)-1
mloop:  ldy     MikeyInitReg,x
        lda     MikeyInitData,x
        sta     $fd00,y
        dex
        bpl     mloop

; These are RAM-shadows of read-only regs.

        ldx     #$1b
        stx     __iodat
        dex                     ; $1A
        stx     __iodir
        ldx     #$d
        stx     __viddma

; Init Suzy.

        ldx     #.sizeof(SuzyInitReg)-1
sloop:  ldy     SuzyInitReg,x
        lda     SuzyInitData,x
        sta     $fc00,y
        dex
        bpl     sloop

        lda     #$24
        sta     __sprsys
        cli

; Clear the BSS data.

        jsr     zerobss

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib         ; Run module destructors

; Endless loop

noret:  bra     noret
