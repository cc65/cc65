/*
  GEOS memory and string functions

  ported to small C on 27.10.1999
  by Maciej 'YTM/Alliance' Witkowiak
*/

#ifndef	_GMEMORY_H
#define _GMEMORY_H

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

void __fastcall__ CopyString(char *dest, char *source);
void __fastcall__ CmpString(char *dest, char *source);
void __fastcall__ CopyFString(char len, char *dest, char *source);
void __fastcall__ CmpFString(char len, char *dest, char *source);

int __fastcall__ CRC(char *buffer, int len);
void __fastcall__ ClearRam(char *dest, int len);
void __fastcall__ FillRam(char what, char *dest, int len);

void __fastcall__ MoveData(char *source, char *dest, int len);

void __fastcall__ InitRam(char *myInitTab);

void __fastcall__ StashRAM(char REUBank, int len, char *reuaddy, char *cpuaddy);
void __fastcall__ FetchRAM(char REUBank, int len, char *reuaddy, char *cpuaddy);
void __fastcall__ SwapRAM(char REUBank, int len, char *reuaddy, char *cpuaddy);
char __fastcall__ VerifyRAM(char REUBank, int len, char *reuaddy, char *cpuaddy);

#endif
