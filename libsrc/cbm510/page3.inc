;
; Page 3 variables for the CBM 610
;
; Taken from a kernal disassembly done by myself in 1987.
;
; Ullrich von Bassewitz, 28.09.1998


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
rvsFlag		     = $0397
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
FunKeyTmp	     = $03B7
sedt3		     = $03B9
MoniSegSave	     = $03f0
wstvec		     = $03F8
WstFlag		     = $03FA	; Warm start flag


