;
; I/O definitions for the CBM 610
;
; Taken from a kernal disassembly done by myself in 1987.
;
; Ullrich von Bassewitz, 28.09.1998


; I/O  $d800: CRTC 6545

;	crtc		=	$d800

 	CAdrReg		=	$00
 	CDataReg	=	$01



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




