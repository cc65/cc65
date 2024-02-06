;
;GEOS Disk Driver JumpTab
;reassembled by Maciej 'YTM/Alliance' Witkowiak
;4-2-99

;pointers
InitForIO_              =       $9000
DoneWithIO_             =       $9002
ExitTurbo_              =       $9004
PurgeTurbo_             =       $9006
EnterTurbo_             =       $9008
ChangeDiskDevice_       =       $900a
NewDisk_                =       $900c
ReadBlock_              =       $900e
WriteBlock_             =       $9010
VerWriteBlock_          =       $9012
OpenDisk_               =       $9014
GetBlock_               =       $9016
PutBlock_               =       $9018
GetDirHead_             =       $901a
PutDirHead_             =       $901c
GetFreeDirBlk_          =       $901e
CalcBlksFree_           =       $9020
FreeBlock_              =       $9022
SetNextFree_            =       $9024
FindBAMBit_             =       $9026
NxtBlkAlloc_            =       $9028
BlkAlloc_               =       $902a
ChkDkGEOS_              =       $902c
SetGEOSDisk_            =       $902e

;jump table
Get1stDirEntry          =       $9030
GetNxtDirEntry          =       $9033
GetBorder               =       $9036
AddDirBlock             =       $9039
ReadBuff                =       $903c
WriteBuff               =       $903f
;??? = $9042
;??? = $9045
AllocateBlock           =       $9048
ReadLink                =       $904b
