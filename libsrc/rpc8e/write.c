// -----------------------------------------------------------------------------
// write.c
//
// write() for cc65-rpc8e
// -----------------------------------------------------------------------------
#include <conio.h>
#include <rpc8e.h>

int __fastcall__ write ( int fd, const void* buf, unsigned count )
{
    unsigned i = -1 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	
	while (++i < count) {
		//Various escape sequence checks.
		//CR check
		if (cbuf[i] == '\r') {
			UINT8_CHARX = 0;
			continue;
		}
		else if (cbuf[i] == '\n') { //LF check
			UINT8_CHARX = 0;
			UINT8_CHARY++;
			UINT8_MEMORYROW++;
			continue;			
		}
		//TODO scrolling check		
		cputc(cbuf[i]);
    }
    
    return count;
}

