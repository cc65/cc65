/*
  GEOS processes (~multitasking) functions

  ported to small C on 27.10.1999
  by Maciej 'YTM/Elysium' Witkowiak
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

void __fastcall__ Sleep(unsigned jiffies);

#endif
