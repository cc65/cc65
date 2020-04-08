// -----------------------------------------------------------------------------
// write.c
//
// write() for cc65-rpc8e
// -----------------------------------------------------------------------------
#include <conio.h>

int __fastcall__ write2 ( int fd, const void* buf, unsigned count )
{
    unsigned i = 0 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	
	while (i < count) {
		//NewLine if check
		cputc(cbuf[i]);
		i++;				
    }
    
    return count;
}

