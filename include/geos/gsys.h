/*
  GEOS system functions

  ported to small C on 27.10.1999
  by Maciej 'YTM/Alliance' Witkowiak
*/

#ifndef	_GSYS_H
#define _GSYS_H

void __fastcall__ FirstInit(void);
void __fastcall__ InitForIO(void);
void __fastcall__ DoneWithIO(void);
void __fastcall__ MainLoop(void);
void __fastcall__ EnterDeskTop(void);
void __fastcall__ ToBASIC(void);
void __fastcall__ Panic(void);

void __fastcall__ CallRoutine(void *myRoutine);

int __fastcall__ GetSerialNumber(void);
char __fastcall__ GetRandom(void);

void __fastcall__ SetDevice(char newdev);

#endif
