;
; Zero page variables for the CBM P500
;
; Taken from a kernal disassembly done by myself in 2000/2001.
;
; Ullrich von Bassewitz, 13.09.2001


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
BlinkOn		= $E6
BlinkCounter	= $E7
ColorRamPtr	= $E8
TempColor	= $EA
BlinkSwitch	= $EB
CHARCOLOR	= $EC
CursBackColor	= $ED		; Color behind cursor
OutCharTmp	= $EE
ScreenSeq	= $EF		; Segment of video RAM

