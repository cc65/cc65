;
; Zero page variables and I/O definitions for the CBM 510
;
; Taken from a kernal disassembly done by myself in 2000/2001.
;
; Ullrich von Bassewitz, 13.09.2001


;-----------------------------------------------------------------------------
; Zeropage stuff

ExecReg		= $0000
IndReg		= $0001

; Up to $20 and $60-8F used by runtime and fixed values
; -----------------------------------

KbdScanBuf     	= $20  	 	; Intermediate for keyboard scan
; RS232 stuff
RecvHead  	= $21		; Head of receive buffer
RecvTail  	= $22		; Tail of receive buffer
RecvFreeCnt	= $23		; Number of bytes in receive buffer
SendHead  	= $24		; Head of send buffer
SendTail  	= $25		; Tail of send buffer
SendFreeCnt	= $26		; Number of bytes free in send buffer

FileNameAdrLo  	= $90
FileNameAdrHi  	= $91
FileNameAdrSeg 	= $92
SaveAdrLow     	= $93
SaveAdrHi      	= $94
SaveAdrSeg     	= $95
EndAdrLow      	= $96
EndAdrHi       	= $97
EndAdrSeg      	= $98
StartAdrLow    	= $99
StartAdrHi     	= $9A
StartAdrSeg    	= $9B
Status 	       	= $9C
FileNameLen    	= $9D
LogicalAdr     	= $9E
FirstAdr       	= $9F
SecondAdr      	= $A0
DefInpDev      	= $A1
DefOutDev      	= $A2
TapeBufPtr     	= $A3
TapeBufPtrSeg  	= $A5
rs232BufPtr    	= $A6
rs232BufPtrSeg 	= $A8
StopKeyFlag    	= $A9
CTemp  	       	= $AA
snsw1  	       	= $AB
SegChgPtr      	= $AC
PChighSave     	= $AE
PClowSave      	= $AF
SRSave 	       	= $B0
ACSave 	       	= $B1
XRSave 	       	= $B2
YRSave 	       	= $B3
SPSave 	       	= $B4
IndSegSave     	= $B5
IRQSaveHi      	= $B7
IRQSaveLo      	= $B8
Adr1   	       	= $B9
Adr2   	       	= $BB
MoniCntr       	= $BD
MoniTmp	       	= $BE
MoniDevNr      	= $BF
PgmKeyBuf      	= $C0
PgmKeyPtr      	= $C2
sedsal 	       	= $C4
sedeal 	       	= $C6
SCREEN_PTR	= $C8
CURS_Y 	       	= $CA
CURS_X 	       	= $CB
GrafMode       	= $CC
LastIndex      	= $CD
LastLine       	= $CE
LastCol	       	= $CF
crsw   	       	= $D0
KeyIndex       	= $D1
QuoteSw	       	= $D2
Insrt  	       	= $D3
Config 	       	= $D4
LastLinePos    	= $D5
PgmKeyIndex    	= $D6
RepeatCount    	= $D7
RepeatDelay    	= $D8
sedt1  	       	= $D9		; Temp
sedt2  	       	= $DA		; Temp, frequently used
PrtData	       	= $DB
ScreenTop      	= $DC
ScreenBot      	= $DD
ScreenLeft     	= $DE
ScreenRight    	= $DF
ModKey 	       	= $E0
NorKey 	       	= $E1
BitTable       	= $E2
CURS_FLAG	= $E6		; 1 = no cursor
CURS_BLINK	= $E7		; cursor blink counter
CRAM_PTR	= $E8
TempColor	= $EA
CURS_STATE	= $EB		; Cursor blink state
CHARCOLOR	= $EC
CURS_COLOR	= $ED		; Color behind cursor
OutCharTmp	= $EE
ScreenSeq	= $EF		; Segment of video RAM

;-----------------------------------------------------------------------------
; Page 3 variables

;
; system ram vectors
;

IRQVec		     = $0300
BRKVec		     = $0302
NMIVec		     = $0304
openVec		     = $0306
closeVec	     = $0308
chkinVec	     = $030A
ckoutVec	     = $030C
clrchVec	     = $030E
basinVec	     = $0310
bsoutVec	     = $0312
stopVec		     = $0314
getinVec	     = $0316
clallVec	     = $0318
loadVec		     = $031A
saveVec		     = $031C
usrcmd		     = $031E
escvec		     = $0320
ctrlvec		     = $0322
secndVec	     = $0324
tksaVec		     = $0326
acptrVec	     = $0328
cioutVec	     = $032A
untlkVec	     = $032C
unlsnVec	     = $032E
listnVec	     = $0330
talkVec		     = $0332

;
;
;

LogicalAdrTable      = $0334
FirstAdrTable	     = $033E
SecondAdrTable	     = $0348
SysMemBot	     = $0352
SysMemTop	     = $0355
UsrMemBot	     = $0358
UsrMemTop	     = $035B
TimOut		     = $035E
VerifyFlag	     = $035F
DevTabIndex	     = $0360
MsgFlag		     = $0361
CassBufPtr	     = $0362
t1		     = $0363
t2		     = $0364
XSave		     = $0365
SaveX		     = $0366
SaveXt		     = $0367
temp		     = $0368
alarm		     = $0369
TapeVec		     = $036A
LoadStAdr	     = $036F
CassMotFlag	     = $0375
m6551Ctrl	     = $0376
m6551Cmd	     = $0377
rs232status	     = $037A
dcddsr		     = $037B
rs232head	     = $037C
rs232tail	     = $037D
PgmKeyEnd	     = $0380
PgmKeySeg	     = $0382
RVS		     = $0383
linetmp		     = $0398
LastPrtChar	     = $0399
InsertFlag	     = $039A
ScrollFlag	     = $039B
FktTemp		     = $039C
PgmKeyIdx	     = $039D
LogScrollFlag	     = $039E
BellMode	     = $039F	; Bell on/off 00 = an
SegSave		     = $03A0
TabStopTable	     = $03A1	; 80 bits for tabstops
KeyBuf		     = $03AB	; Keyboard buffer
funvec		     = $03B5	; Vector for function key handline
sedt3		     = $03B9
MoniSegSave	     = $03f0
wstvec		     = $03F8
WstFlag		     = $03FA	; Warm start flag

; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 40
YSIZE           = 25

;-----------------------------------------------------------------------------
; I/O Definitions

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
VIC_SPR_EXP_Y  	= $17
VIC_SPR_EXP_X  	= $1D
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

