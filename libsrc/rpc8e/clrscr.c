#include <rpc8e.h>


void clrscr (void) {
	UINT8_BLIT_START_X = ' '; //Also stores blit source when filling
	UINT8_BLIT_END_X =  0;
	UINT8_BLIT_END_Y =  0;
	UINT8_BLIT_WIDTH =  80;
	UINT8_BLIT_HEIGHT = 50;
	UINT8_BLIT_CMD = BLITCMD_MONITOR_FILL;
	UINT8_CHARX = 0;
	UINT8_CHARY = 0;
	UINT8_MEMORYROW = 0;
	waitTick(); //wai, 65816 borrowed opcode, the blitter finishes on the begining of the next opcode
	return;
}