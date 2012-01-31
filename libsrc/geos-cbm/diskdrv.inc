
;GEOS Disk Driver JumpTab
;reassembled by Maciej 'YTM/Alliance' Witkowiak
;4-2-99

;pointers
_InitForIO		=	$9000
_DoneWithIO		=	$9002
_ExitTurbo		=	$9004
_PurgeTurbo		=	$9006
_EnterTurbo		=	$9008
_ChangeDiskDevice	=	$900a
_NewDisk		=	$900c
_ReadBlock		=	$900e
_WriteBlock		=	$9010
_VerWriteBlock		=	$9012
_OpenDisk		=	$9014
_GetBlock		=	$9016
_PutBlock		=	$9018
_GetDirHead		=	$901a
_PutDirHead		=	$901c
_GetFreeDirBlk		=	$901e
_CalcBlksFree		=	$9020
_FreeBlock		=	$9022
_SetNextFree		=	$9024
_FindBAMBit		=	$9026
_NxtBlkAlloc		=	$9028
_BlkAlloc		=	$902a
_ChkDkGEOS		=	$902c
_SetGEOSDisk		=	$902e

;jump table
Get1stDirEntry		=	$9030
GetNxtDirEntry		=	$9033
GetBorder		=	$9036
AddDirBlock		=	$9039
ReadBuff		=	$903c
WriteBuff		=	$903f
;??? = $9042
;??? = $9045
AllocateBlock		=	$9048
ReadLink		=	$904b
