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

FNAM            := $90          ; File name address
FNAM_SEG        := $92          ; Unused
SaveAdrLow     	= $93
SaveAdrHi      	= $94
SaveAdrSeg     	= $95
EndAdrLow      	= $96
EndAdrHi       	= $97
EndAdrSeg      	= $98
StartAdrLow    	= $99
StartAdrHi     	= $9A
StartAdrSeg    	= $9B
ST              := $9C          ; Status
FNAM_LEN        := $9D
LFN             := $9E
DEVNUM          := $9F
SECADR          := $A0
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
CharPtr	       	= $C8
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


; ---------------------------------------------------------------------------
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
PgmKeySize	     = $0383
RVS		     = $0397
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
FUNKEY_VEC     	     = $03B5    ; Vector for function key handline
FunKeyTmp	     = $03B7
sedt3		     = $03B9
MoniSegSave	     = $03f0
wstvec		     = $03F8
WstFlag		     = $03FA	; Warm start flag


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


