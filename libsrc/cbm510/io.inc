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

; 	IPCcia		=	$db00

 	PortA		=	$00
 	PortB		=	$01
 	DDRA		=	$02
 	DDRB		=	$03
 	TimALo		=	$04
 	TimAHi		=	$05
 	TimBLo		=	$06
 	TimBHi		=	$07
 	TOD10		=	$08
 	TODsec		=	$09
 	TODmin		=	$0A
 	TODhour		=	$0B
 	SerDataReg	=	$0C
 	IntCtrReg	=	$0D
 	CtrlA		=	$0E
 	CtrlB		=	$0F



; I/O  $dc00: CIA 6526

; 	cia		=	$dc00



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




