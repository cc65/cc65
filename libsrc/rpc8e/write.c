// -----------------------------------------------------------------------------
// write.c
//
// write() for cc65-rpc8e
// -----------------------------------------------------------------------------
#include <conio.h>

#define UINT8_MEMORYROW (*(unsigned char*)0x0300)
#define UINT8_CHARX     (*(unsigned char*)0x0301)
#define UINT8_CHARY     (*(unsigned char*)0x0302)
#define PTR_LINEBUFFER  ((unsigned char*)0x0310)

int __fastcall__ write ( int fd, const void* buf, unsigned count )
{
    unsigned i = 0 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	
	while (i < count) {
		//NewLine if check
		/*
		if (cbuf[i] == '\n') {
			UINT8_CHARX = 0;
			UINT8_CHARY++;
			UINT8_MEMORYROW++;
			i++; //CR
			i++; //LF
			continue;
		} else if (UINT8_CHARX >= 80) {
			UINT8_CHARX = 0;
			UINT8_CHARY++;
			UINT8_MEMORYROW++;
		}
		curLine[UINT8_CHARX] = cbuf[i];
		UINT8_CHARX++;
		*/
		cputc(cbuf[i]);
		i++;
				
    }
    
    return count;
}
