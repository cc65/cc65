;
; I/O definitions for the CBM 510
;
; Taken from a kernal disassembly done by myself in 2000/2001.
;
; Ullrich von Bassewitz, 13.09.2001


; I/O  $d800: VIC

VIC_SPR0_X     	= $00
VIC_SPR0_Y     	= $01
VIC_SPR1_X     	= $02
VIC_SPR1_Y     	= $03
VIC_SPR2_X     	= $04
VIC_SPR2_Y     	= $05
VIC_SPR3_X     	= $06
VIC_SPR3_Y     	= $07
VIC_SPR4_X     	= $08
VIC_SPR4_Y     	= $09
VIC_SPR5_X     	= $0A
VIC_SPR5_Y     	= $0B
VIC_SPR6_X     	= $0C
VIC_SPR6_Y     	= $0D
VIC_SPR7_X     	= $0E
VIC_SPR7_Y     	= $0F
VIC_SPR_HI_X   	= $10
VIC_SPR_ENA    	= $15
VIC_SPR_EXP_X  	= $17
VIC_SPR_EXP_Y  	= $1D
VIC_SPR_MCOLOR 	= $1C
VIC_SPR_BG_PRIO = $1B

VIC_SPR_MCOLOR0 = $25
VIC_SPR_MCOLOR1 = $26

VIC_SPR0_COLOR 	= $27
VIC_SPR1_COLOR 	= $28
VIC_SPR2_COLOR 	= $29
VIC_SPR3_COLOR 	= $2A
VIC_SPR4_COLOR 	= $2B
VIC_SPR5_COLOR 	= $2C
VIC_SPR6_COLOR 	= $2D
VIC_SPR7_COLOR 	= $2E

VIC_CTRL1      	= $11
VIC_CTRL2      	= $16

VIC_HLINE      	= $12

VIC_VIDEO_ADR  	= $18

VIC_IRR	       	= $19		; Interrupt request register
VIC_IMR	       	= $1A		; Interrupt mask register

VIC_BORDERCOLOR = $20
VIC_BG_COLOR0  	= $21
VIC_BG_COLOR1  	= $22
VIC_BG_COLOR2  	= $23
VIC_BG_COLOR3  	= $24



; I/O  $da00: SID 6581

SID_S1Lo       	= $00
SID_S1Hi       	= $01
SID_PB1Lo      	= $02
SID_PB1Hi      	= $03
SID_Ctl1       	= $04
SID_AD1	       	= $05
SID_SUR1       	= $06

SID_S2Lo       	= $07
SID_S2Hi       	= $08
SID_PB2Lo      	= $09
SID_PB2Hi      	= $0A
SID_Ctl2       	= $0B
SID_AD2	       	= $0C
SID_SUR2       	= $0D

SID_S3Lo       	= $0E
SID_S3Hi       	= $0F
SID_PB3Lo      	= $10
SID_PB3Hi      	= $11
SID_Ctl3       	= $12
SID_AD3	       	= $13
SID_SUR3       	= $14

SID_FltLo      	= $15
SID_FltHi      	= $16
SID_FltCtl     	= $17
SID_Amp	       	= $18
SID_ADConv1    	= $19
SID_ADConv2    	= $1A
SID_Noise      	= $1B
SID_Read3      	= $1C


; I/O  $db00: CIA 6526 Inter Process Communication
; I/O  $dc00: CIA 6526

CIA_PRA	        = $00
CIA_PRB		= $01
CIA_DDRA       	= $02
CIA_DDRB	= $03
CIA_ICR		= $0D
CIA_CRA		= $0E
CIA_CRB		= $0F


; I/O  $dd00: ACIA 6551

; 	acia		=	$dd00

 	ADataReg	=	$00
 	AStatusReg	=	$01
 	ACmdReg		=	$02
 	ACtrlReg	=	$03



; I/O  $de00: Triport #1 6525

; 	tpi1		=	$de00

 	tpiPortA	=	$00
 	tpiPortB	=	$01
 	tpiPortC	=	$02
 	tpiIntLatch	=	$02
 	tpiDDRA		=	$03
 	tpiDDRB		=	$04
 	tpiDDRC		=	$05
 	tpiIntMask	=	$05
 	tpiCtrlReg	=	$06
 	tpiActIntReg	=	$07



; I/O  $df00: Triport #2 6525

; 	tpi2		=	$df00



; Out video memory address

COLOR_RAM   = $D400			; System bank

