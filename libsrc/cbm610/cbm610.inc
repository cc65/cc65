;
; Zeropage and I/O definitions for the CBM 610
;
; Taken from a kernal disassembly done by myself in 1987.
;
; Ullrich von Bassewitz, 28.09.1998


; ---------------------------------------------------------------------------
; Zeropage stuff

ExecReg		= $0000
IndReg		= $0001

FNAM_ADR        := $90          ; File name address
FNAM_SEG        := $92          ; Unused
ST              := $9C          ; Status
FNAM_LEN        := $9D
LFN             := $9E
DEVNUM          := $9F
SECADR          := $A0
;CharPtr	       	= $C8
;CURS_Y 	       	= $CA
;CURS_X 	       	= $CB
GrafMode       	= $CC
LastIndex      	= $CD
KeyIndex       	= $D1
Config 	       	= $D4
RepeatCount    	= $D7
RepeatDelay    	= $D8
ModKey 	       	= $E0
NorKey 	       	= $E1


; ---------------------------------------------------------------------------
; Page 3 variables

RVS		     = $0397
KeyBuf		     = $03AB	; Keyboard buffer


; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 80
YSIZE           = 25

; ---------------------------------------------------------------------------
; I/O definitions


; I/O  $d800: CRTC 6545

CRTC		= $D800
CRTC_ADDR	= $00
CRTC_DATA	= $01


; I/O  $da00: SID 6581

; 	sid		=	$da00

Osc1		=	$00
Osc2		=	$07
Osc3		=	$0e

FreqLo		=	$00
FreqHi		=	$01
PulseF		=	$02
PulseC		=	$03
OscCtl		=	$04
AtkDcy		=	$05
SusRel		=	$06

FiCtlLo		=	$15
FiCtlHi		=	$16
Resonance	=	$17
Volume		=	$18
PotX		=	$19
PotY		=	$1A
Random		=	$1B
Env3		=	$1C



; I/O  $db00: CIA 6526 Inter Process Communication
;
; 	IPCcia		=	$db00

.struct CIA
        PRA     .byte
        PRB     .byte
        DDRA    .byte
        DDRB    .byte
        .union
            .struct
                TALO    .byte
                TAHI    .byte
            .endstruct
            TA          .word
        .endunion
        .union
            .struct
                TBLO    .byte
                TBHI    .byte
            .endstruct
            TB          .word
        .endunion
        TOD10   .byte
        TODSEC  .byte
        TODMIN  .byte
        TODHR   .byte
        SDR     .byte
        ICR     .byte
        CRA     .byte
        CRB     .byte
.endstruct


; I/O  $dc00: CIA 6526

; 	cia		=	$dc00



; I/O  $dd00: ACIA 6551

; 	acia	       	=	$dd00

.struct ACIA
        DATA    .byte
        STATUS  .byte
        CMD     .byte
        CTRL    .BYTE
.endstruct


; I/O  $de00: Triport #1 6525

; 	tpi1  		=	$de00

.struct TPI
        PRA     .byte
        PRB     .byte
        .union
                PRC     .byte
                INT     .byte
        .endunion
        DDRA    .byte
        DDRB    .byte
        .union
                DDRC    .byte
                IMR     .byte
        .endunion
        CR      .byte
        AIR     .byte
.endstruct


; I/O  $df00: Triport #2 6525

; 	tpi2	     	=	$df00


