// -----------------------------------------------------------------------------
// write.c
//
// write() for cc65-rpc8e
// -----------------------------------------------------------------------------


#include <unistd.h>
#include <conio.h>
#include <peekpoke.h>

#define PTR_LINEBUFFER ((unsigned char*)0x0310)

int write ( int fd, const void* buf, unsigned count )
{
    unsigned i = 0 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	/*
	unsigned char *curLine = PTR_LINEBUFFER;
    unsigned i = 0 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	unsigned char *arbitaryWrite;
	*/
	/*
    while (i < count) {
        curLine[i] = cbuf[i];
        i = i + 1;
    }
	*/
	/*
	arbitaryWrite = (unsigned char *) 0x0310;
	arbitaryWrite[0] = cbuf[0];
	*/

    return count;
}
