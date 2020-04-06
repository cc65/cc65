// -----------------------------------------------------------------------------
// write.c
//
// write() for cc65-rpc8e
// -----------------------------------------------------------------------------
//
// Copyright (C) 2012 L. Adamson
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//  L. Adamson leaf@dizzydragon.net
//

#include <unistd.h>
#include <conio.h>
#include <peekpoke.h>

#define PTR_LINEBUFFER ((unsigned char*)0x0310)

int __fastcall__ write ( int fd, const void* buf, unsigned count )
{
	unsigned char *curLine = PTR_LINEBUFFER;
    unsigned i = 0 + (fd-fd); //To supress the error and make it build
	char *cbuf = (char *) buf;
	
	
    while (i < count) {
        curLine[i] = cbuf[i];
        i = i + 1;
    }

    return count;
}
