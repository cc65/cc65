/*
  GEOS menu and icon functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GMENU_H
#define _GMENU_H

#include <geos/gstruct.h>

void __fastcall__ DoMenu(struct menu *myMenu);
void ReDoMenu(void);
void RecoverMenu(void);
void RecoverAllMenus(void);
void DoPreviousMenu(void);
void GotoFirstMenu(void);

void __fastcall__ DoIcons(struct icontab *myIconTab);

/* DoMenu - menutypes */
#define MENU_ACTION     0x00
#define DYN_SUB_MENU    0x40
#define SUB_MENU        0x80
#define HORIZONTAL      0x00
#define VERTICAL        0x80
/* menu string offsets */
#define OFF_MY_TOP      0
#define OFF_MY_BOT      1
#define OFF_MX_LEFT     2
#define OFF_MX_RIGHT    4
#define OFF_NUM_M_ITEMS 6
#define OFF_1ST_M_ITEM  7
/* icon string offsets */
#define OFF_NM_ICNS     0
#define OFF_IC_XMOUSE   1
#define OFF_IC_YMOUSE   3
#define OFF_PIC_ICON    0
#define OFF_X_ICON_POS  2
#define OFF_Y_ICON_POS  3
#define OFF_WDTH_ICON   4
#define OFF_HEIGHT_ICON 5
#define OFF_SRV_RT_ICON 6
#define OFF_NX_ICON     8
/* icons, menus status flags    */
#define ST_FLASH        0x80
#define ST_INVERT       0x40
#define ST_LD_AT_ADDR   0x01
#define ST_LD_DATA      0x80
#define ST_PR_DATA      0x40
#define ST_WR_PR        0x40

#endif
