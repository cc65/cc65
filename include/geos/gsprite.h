/*
  GEOS mouse and sprite functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GSPRITE_H
#define _GSPRITE_H

void StartMouseMode(void);
void ClearMouseMode(void);
void MouseUp(void);
void MouseOff(void);
char __fastcall__ IsMseInRegion(struct window *region);

void __fastcall__ DrawSprite(char spritenum, const char *spritepic);
void __fastcall__ PosSprite(char spritenum, struct pixel *position);
void __fastcall__ EnablSprite(char spritenum);
void __fastcall__ DisablSprite(char spritenum);

void __fastcall__ InitTextPrompt(char height);
void __fastcall__ PromptOn(struct pixel *position);
void PromptOff(void);
char GetNextChar(void);

/* keyboard constants */
#define KEY_F1          1
#define KEY_F2          2
#define KEY_F3          3
#define KEY_F4          4
#define KEY_F5          5
#define KEY_F6          6
#define KEY_NOSCRL      7
#define KEY_ENTER       13
#define KEY_F7          14
#define KEY_F8          15
#define KEY_HOME        18
#define KEY_CLEAR       19
#define KEY_LARROW      20
#define KEY_UPARROW     21
#define KEY_STOP        22
#define KEY_RUN         23
#define KEY_BPS         24
#define KEY_HELP        25
#define KEY_ALT         26
#define KEY_ESC         27
#define KEY_INSERT      28
#define KEY_INVALID     31
#define KEY_LEFT        BACKSPACE
#ifdef __GEOS_CBM__
#define KEY_UP          16
#define KEY_DOWN        17
#define KEY_DELETE      29
#define KEY_RIGHT       30
#else
#define KEY_UP          11
#define KEY_DOWN        10
#define KEY_DELETE     127
#define KEY_RIGHT       21
#endif

/* values of faultData - pointer position vs. mouseWindow */
/* bit numbers */
#define OFFTOP_BIT      7
#define OFFBOTTOM_BIT   6
#define OFFLEFT_BIT     5
#define OFFRIGHT_BIT    4
#define OFFMENU_BIT     3
/* bit masks */
#define SET_OFFTOP      0x80
#define SET_OFFBOTTOM   0x40
#define SET_OFFLEFT     0x20
#define SET_OFFRIGHT    0x10
#define SET_OFFMENU     0x08

/* mouseOn */
/* bit numbers */
#define MOUSEON_BIT     7
#define MENUON_BIT      6
#define ICONSON_BIT     5
/* bit masks */
#define SET_MSE_ON      0x80
#define SET_MENUON      0x40
#define SET_ICONSON     0x20

/* pressFlag */
/* bit numbers */
#define KEYPRESS_BIT    7
#define INPUT_BIT       6
#define MOUSE_BIT       5
/* bit masks */
#define SET_KEYPRESS    0x80
#define SET_INPUTCHG    0x40
#define SET_MOUSE       0x20

#endif
