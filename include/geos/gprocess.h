/*
  GEOS processes (~multitasking) functions

  ported to small C on 27.10.1999
  by Maciej 'YTM/Alliance' Witkowiak
*/

#ifndef	_GPROCESS_H
#define _GPROCESS_H

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

void __fastcall__ InitProcesses(char number, struct process *proctab);
void __fastcall__ RestartProcess(char number);
void __fastcall__ EnableProcess(char number);
void __fastcall__ BlockProcess(char number);
void __fastcall__ UnBlockProcess(char number);
void __fastcall__ FreezeProcess(char number);
void __fastcall__ UnFreezeProcess(char number);

void __fastcall__ Sleep(int jiffies);

/*  Process control variable
    these probably should be removed from here, as they are
    internal GEOS information which is updated by functions above
*/

/* bit numbers */
#define	RUNABLE_BIT	7
#define	BLOCKED_BIT	6
#define	FROZEN_BIT	5
#define	NOTIMER_BIT	4
/* bit masks */
#define	SET_RUNABLE	0x80
#define	SET_BLOCKED	0x40
#define	SET_FROZEN	0x20
#define	SET_NOTIMER	0x10

#endif
