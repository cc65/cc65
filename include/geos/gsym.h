/*
  GEOS system addresses and locations

  reassembled by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GSYM_H
#define _GSYM_H

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

#define nameBuf		char[17]
#define blockBuf	char[256]

#define	zpage		((blockBuf)0x0000)

#define	CPU_DDR		*(char*)0x00
#define	CPU_DATA	*(char*)0x01

#define	r0		*(unsigned int*)0x02
#define	r0L		*(char*)0x02
#define	r0H		*(char*)0x03
#define	r1		*(unsigned int*)0x04
#define	r1L		*(char*)0x04
#define	r1H		*(char*)0x05
#define drawWindow	(*(struct window*)0x06)
#define	r2		*(unsigned int*)0x06
#define	r2L		*(char*)0x06
#define	r2H		*(char*)0x07
#define	r3		*(unsigned int*)0x08
#define	r3L		*(char*)0x08
#define	r3H		*(char*)0x09
#define	r4		*(unsigned int*)0x0a
#define	r4L		*(char*)0x0a
#define	r4H		*(char*)0x0b
#define	r5		*(unsigned int*)0x0c
#define	r5L		*(char*)0x0c
#define	r5H		*(char*)0x0d
#define	r6		*(unsigned int*)0x0e
#define	r6L		*(char*)0x0e
#define	r6H		*(char*)0x0f
#define	r7		*(unsigned int*)0x10
#define	r7L		*(char*)0x10
#define	r7H		*(char*)0x11
#define	r8		*(unsigned int*)0x12
#define	r8L		*(char*)0x12
#define	r8H		*(char*)0x13
#define	r9		*(unsigned int*)0x14
#define	r9L		*(char*)0x14
#define	r9H		*(char*)0x15
#define	r10		*(unsigned int*)0x16
#define	r10L		*(char*)0x16
#define	r10H		*(char*)0x17
#define	r11		*(unsigned int*)0x18
#define	r11L		*(char*)0x18
#define	r11H		*(char*)0x19
#define	r12		*(unsigned int*)0x1a
#define	r12L		*(char*)0x1a
#define	r12H		*(char*)0x1b
#define	r13		*(unsigned int*)0x1c
#define	r13L		*(char*)0x1c
#define	r13H		*(char*)0x1d
#define	r14		*(unsigned int*)0x1e
#define	r14L		*(char*)0x1e
#define	r14H		*(char*)0x1f
#define	r15		*(unsigned int*)0x20
#define	r15L		*(char*)0x20
#define	r15H		*(char*)0x21

#define	curPattern	*(unsigned int*)0x22
#define	string		*(unsigned int*)0x24
#define curFontDesc	(*(struct fontdesc*)0x26)
#define	currentMode	*(char*)0x2e
#define	dispBufferOn	*(char*)0x2f
#define	mouseOn		*(char*)0x30
#define	msePicPtr	*(unsigned int*)0x31
#define curWindow	(*(struct window*)0x33)
#define	pressFlag	*(char*)0x39
#define mousePos	(*(struct pixel*)0x3a)
#define	returnAddress	*(unsigned int*)0x3d
#define	graphMode	*(char*)0x3f
#define	STATUS		*(char*)0x90
#define	curDevice	*(char*)0xba

#define	irqvec		(*(void_func*)0x0314)
#define	bkvec		(*(void_func*)0x0316)
#define	nmivec		(*(void_func*)0x0318)

#define	APP_RAM		(char*)0x0400
#define	BACK_SCR_BASE	(char*)0x6000
#define	PRINTBASE	(char*)0x7900
#define	OS_VARS		(char*)0x8000

#define	diskBlkBuf	((blockBuf)0x8000)
#define fileHeader	(*(struct fileheader*)0x8100)
#define curDirHead	((blockBuf)0x8200)
#define fileTrScTab	((struct tr_se[128])0x8300)
#define dirEntryBuf	(*(struct filehandle*)0x8400)

#define DrACurDkNm	((nameBuf)0x841e)
#define DrBCurDkNm	((nameBuf)0x8430)
#define dataFileName	((nameBuf)0x8442)
#define dataDiskName	((nameBuf)0x8453)
#define PrntFileName	((nameBuf)0x8465)
#define PrntDiskName	((nameBuf)0x8476)

#define	curDrive	*(char*)0x8489
#define	diskOpenFlg	*(char*)0x848a
#define	isGEOS		*(char*)0x848b
#define	interleave	*(char*)0x848c
#define	NUMDRV		*(char*)0x848d

#define driveType	((char[4])0x848e)
#define turboFlags	((char[4])0x8492)

#define	VLIRInfo	(*(struct VLIR_info*)0x8496)

#define appMain		(*(void_func*)0x849b)
#define intTopVector	(*(void_func*)0x849d)
#define intBotVector	(*(void_func*)0x849f)
#define mouseVector	(*(void_func*)0x84a1)
#define keyVector	(*(void_func*)0x84a3)
#define	inputVector	(*(void_func*)0x84a5)
#define	mouseFaultVec	(*(void_func*)0x84a7)
#define	otherPressVec	(*(void_func*)0x84a9)
#define	StringFaultVec	(*(void_func*)0x84ab)
#define	alarmTmtVector	(*(void_func*)0x84ad)
#define	BRKVector	(*(void_func*)0x84af)
#define	RecoverVector	(*(void_func*)0x84b1)

#define	selectionFlash	*(char*)0x84b3
#define	alphaFlag	*(char*)0x84b4
#define	iconSelFlg	*(char*)0x84b5
#define	faultData	*(char*)0x84b6
#define	menuNumber	*(char*)0x84b7
#define mouseWindow	(*(struct window*)0x84b8)
#define stringXY	(*(struct pixel*)0x84be)
#define	mousePicData	*(char*)0x84c1

#define	maxMouseSpeed	*(char*)0x8501
#define	minMouseSpeed	*(char*)0x8502
#define	mouseAccel	*(char*)0x8503
#define	keyData		*(char*)0x8504
#define	mouseData	*(char*)0x8505
#define	inputData	*(char*)0x8506
#define	mouseSpeed	*(char*)0x8507
#define	random		*(char*)0x850a
#define	saveFontTab	(*(struct fontdesc*)0x850c)

#define	dblClickCount	*(char*)0x8515
#define system_date	(*(struct s_date*)0x8516)
#define	alarmSetFlag	*(char*)0x851c
#define	sysDBData	*(char*)0x851d
#define	screencolors	*(char*)0x851e
#define	dlgBoxRamBuf	*(char*)0x851f

#define	savedmoby2	*(char*)0x88bb
#define	scr80polar	*(char*)0x88bc
#define	scr80colors	*(char*)0x88bd
#define	vdcClrMode	*(char*)0x88be
#define	driveData	((char[4])0x88bf)
#define	ramExpSize	*(char*)0x88c3
#define	sysRAMFlg	*(char*)0x88c4
#define	firstBoot	*(char*)0x88c5
#define	curType		*(char*)0x88c6
#define	ramBase		*(char*)0x88c7
#define inputDevName	((nameBuf)0x88cb)
#define	DrCCurDkNm	((nameBuf)0x88dc)
#define	DrDCurDkNm	((nameBuf)0x88ee)
#define	dir2Head	((blockBuf)0x8900)
#define	SPRITE_PICS	(char*)0x8a00
#define sprpic		((char[8][64])0x8a00)
#define COLOR_MATRIX	((char[1000])0x8c00)
#define objPointer	((char[8])0x8ff8)

#define	DISK_BASE	(char*)0x9000
#define	SCREEN_BASE	(char*)0xa000
#define	OS_ROM		(char*)0xc000
#define	OS_JUMPTAB	(char*)0xc100
#define	EXP_BASE	(char*)0xdf00
#define	MOUSE_BASE_128	(char*)0xfd00
#define	MOUSE_JMP_128	(char*)0xfd00
#define	END_MOUSE_128	(char*)0xfe80
#define	MOUSE_BASE	(char*)0xfe80
#define	MOUSE_JMP	(char*)0xfe80

#define	config		*(char*)0xff00
#define	END_MOUSE	(char*)0xfffa

#define	NMI_VECTOR	(*(void_func*)0xfffa)
#define	RESET_VECTOR	(*(void_func*)0xfffc)
#define	IRQ_VECTOR	(*(void_func*)0xfffe)

#define	vicbase		(char*)0xd000
#define	sidbase		(char*)0xd400
#define	mmu		(char*)0xd500
#define	VDC		(char*)0xd600
#define	ctab		(char*)0xd800
#define	cia1base	(char*)0xdc00
#define	cia2base	(char*)0xdd00

#define	mob0xpos	*(char*)0xd000
#define	mob0ypos	*(char*)0xd001
#define	mob1xpos	*(char*)0xd002
#define	mob1ypos	*(char*)0xd003
#define	mob2xpos	*(char*)0xd004
#define	mob2ypos	*(char*)0xd005
#define	mob3xpos	*(char*)0xd006
#define	mob3ypos	*(char*)0xd007
#define	mob4xpos	*(char*)0xd008
#define	mob4ypos	*(char*)0xd009
#define	mob5xpos	*(char*)0xd00a
#define	mob5ypos	*(char*)0xd00b
#define	mob6xpos	*(char*)0xd00c
#define	mob6ypos	*(char*)0xd00d
#define	mob7xpos	*(char*)0xd00e
#define	mob7ypos	*(char*)0xd00f
#define	msbxpos		*(char*)0xd010
#define	grcntrl1	*(char*)0xd011
#define	rasreg		*(char*)0xd012
#define	lpxpos		*(char*)0xd013
#define	lpypos		*(char*)0xd014
#define	mobenble	*(char*)0xd015
#define	grcntrl2	*(char*)0xd016
#define	grmemptr	*(char*)0xd018
#define	grirq		*(char*)0xd019
#define	grirqen		*(char*)0xd01a
#define	moby2		*(char*)0xd017
#define	mobprior	*(char*)0xd01b
#define	mobmcm		*(char*)0xd01c
#define	mobx2		*(char*)0xd01d
#define	mobmobcol	*(char*)0xd01e
#define	mobbakcol	*(char*)0xd01f
#define	extclr		*(char*)0xd020
#define	bakclr0		*(char*)0xd021
#define	bakclr1		*(char*)0xd022
#define	bakclr2		*(char*)0xd023
#define	bakclr3		*(char*)0xd024
#define	mcmclr0		*(char*)0xd025
#define	mcmclr1		*(char*)0xd026
#define	mob0clr		*(char*)0xd027
#define	mob1clr		*(char*)0xd028
#define	mob2clr		*(char*)0xd029
#define	mob3clr		*(char*)0xd02a
#define	mob4clr		*(char*)0xd02b
#define	mob5clr		*(char*)0xd02c
#define	mob6clr		*(char*)0xd02d
#define	mob7clr		*(char*)0xd02e
#define	keyreg		*(char*)0xd02f
#define	clkreg		*(char*)0xd030

#define	vdcreg		*(char*)0xd600
#define	vdcdata		*(char*)0xd601

#endif
