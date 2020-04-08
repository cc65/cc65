#include <unistd.h>
#include <rpc8e.h>

void __fastcall__ cputc (char c)
{
	unsigned char *curLine = PTR_LINEBUFFER;
	
	//Newline if check
	if ( (c == '\r') || (c == '\n') ) {
		UINT8_CHARX = 0;
		UINT8_CHARY++;
		UINT8_MEMORYROW++;
		return;
	} else if (UINT8_CHARX >= 80) {
		UINT8_CHARX = 0;
		UINT8_CHARY++;
		UINT8_MEMORYROW++;
	}
	curLine[UINT8_CHARX] = c;
	UINT8_CHARX++;
	
	return;
}