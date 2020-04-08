#ifndef _RPC8E_H
#define _RPC8E_H

/* Check for errors */
#ifndef __RPC8E__
#  error This module may be used only when compiling for the rpc8e!
#endif

#define UINT8_MEMORYROW (*(unsigned char*)0x0300)
#define UINT8_CHARX     (*(unsigned char*)0x0301)
#define UINT8_CHARY     (*(unsigned char*)0x0302)
#define PTR_LINEBUFFER  ((unsigned char*)0x0310)
#define UINT8_KEY_CURCHAR_INDEX     (*(unsigned char*)0x0304)
#define UINT8_KEY_NEWCHAR_POS_INDEX (*(unsigned char*)0x0305)
#define UINT8_KEY_READ_REG          (*(unsigned char*)0x0306)



#endif