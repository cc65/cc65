#ifndef _RPC8E_H
#define _RPC8E_H

/* Check for errors */
#ifndef __RPC8E__
#  error This module may be used only when compiling for the rpc8e!
#endif
extern void __fastcall__ waitTick(void);

#define UINT8_MEMORYROW  (*(unsigned char*)0x0300)
#define UINT8_CHARX      (*(unsigned char*)0x0301)
#define UINT8_CHARY      (*(unsigned char*)0x0302)
#define UINT8_CURSORMODE (*(unsigned char*)0x0303)
#define PTR_LINEBUFFER   ((unsigned char*)0x0310)

#define UINT8_KEY_CURCHAR_INDEX     (*(unsigned char*)0x0304)
#define UINT8_KEY_NEWCHAR_POS_INDEX (*(unsigned char*)0x0305)
#define UINT8_KEY_READ_REG          (*(unsigned char*)0x0306)

#define UINT8_BLIT_CMD (*(unsigned char*)0x0307)
#define UINT8_BLIT_START_X (*(unsigned char*)0x0308)
#define UINT8_BLIT_START_Y (*(unsigned char*)0x0309)
#define UINT8_BLIT_END_X   (*(unsigned char*)0x030A)
#define UINT8_BLIT_END_Y   (*(unsigned char*)0x030B)
#define UINT8_BLIT_WIDTH   (*(unsigned char*)0x030C)
#define UINT8_BLIT_HEIGHT  (*(unsigned char*)0x030D)
//0x030E and 0x030F are unused so they now store static variables used by cputc

#define BLITCMD_MONITOR_FILL   0x01
#define BLITCMD_MONITOR_INVERT 0x02
#define BLITCMD_MONITOR_COPY   0x03


#endif