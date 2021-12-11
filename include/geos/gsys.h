/*
  GEOS system functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GSYS_H
#define _GSYS_H

void FirstInit(void);
void InitForIO(void);
void DoneWithIO(void);
void MainLoop(void);
void EnterDeskTop(void);
void ToBASIC(void);
void Panic(void);

void __fastcall__ CallRoutine(void *myRoutine);

unsigned GetSerialNumber(void);
char GetRandom(void);

void __fastcall__ SetDevice(char newdev);

char get_ostype(void);

/* possible return values of get_ostype, machine and version flags will
   be combined with OR */
  /* machine flags */
#define GEOS64          0x00
#define GEOS4           0x04    /* plus4 geos is not or'ed with version */
#define GEOS128         0x80
  /* version flags */
#define MEGAPATCH3      0x03
#define GATEWAY         0x08
#define GEOS_V10        0x10
#define GEOS_V11        0x11
#define GEOS_V12        0x12
#define GEOS_V13        0x13
#define GEOS_V15        0x15
#define GEOS_V20        0x20
#define WHEELS          0x40    /* only Wheels? */

char get_tv(void);

/* possible return values of get_tv, these flags will be combined
   note that columns state can be changed during runtime and get_tv
   always returns the current state */
#define COLUMNS40       0x00
#define COLUMNS80       0x01
#define TV_PAL          0x00
#define TV_NTSC         0x80

#endif
