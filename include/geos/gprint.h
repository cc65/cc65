/*
  GEOS printer driver functions
*/

#ifndef _GPRINT_H
#define _GPRINT_H

#if defined(__GEOS_CBM__)

void InitForPrint(void);
char __fastcall__ StartPrint(char *workBuf);
void __fastcall__ PrintBuffer(char *printData, char *workBuf, char *colorData);
char __fastcall__ StopPrint(char *tempBuf, char *workBuf);
void __fastcall__ GetDimensions(char *width, char *height, char *mode);
char __fastcall__ StartASCII(char *workBuf);
void __fastcall__ PrintASCII(const char *printData, char *workBuf);
void __fastcall__ SetNLQ(char *workBuf);

#endif

#endif
