/*
  GEOS processes (~multitasking) functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GPROCESS_H
#define _GPROCESS_H

#include <geos/gstruct.h>

void __fastcall__ InitProcesses(char number, struct process *proctab);
void __fastcall__ RestartProcess(char number);
void __fastcall__ EnableProcess(char number);
void __fastcall__ BlockProcess(char number);
void __fastcall__ UnblockProcess(char number);
void __fastcall__ FreezeProcess(char number);
void __fastcall__ UnfreezeProcess(char number);

void __fastcall__ Sleep(unsigned jiffies);

#endif
