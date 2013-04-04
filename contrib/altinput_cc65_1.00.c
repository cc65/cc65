

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

char __fastcall__ GetInput (char* In, char InLen)
{
	static unsigned char c; //Current char.
	static char CurPos;	//Current pos./len. in buffer
	CurPos=0;

	//Prompt.  If you desire a different prompt, printf() it
	//here.
	putchar ('>');
	//Main loop:
	while (1)
	{
		//Get key.  If on an Apple, you may need to emulate a 
		//cursor.
		cursor (1);
		c = cgetc ();
		cursor (0);

		//Process the key.
		if (c=='\n'){ /*return*/
			In[CurPos]=0;		//Terminate input string.
			putchar ('\n');		//Go to next screen line.
			return CurPos;		//Return string len.
		} else if (c == '\b' && CurPos > 0) { /* delete */
			//Won't be processed if the buffer is empty.
			--CurPos;		//Cursor left.
			putchar ('\b');		//Delete last char. from screen.
		} else if (isprint(c) && CurPos<InLen) { //Printable char.
			In[CurPos++]=c;		//Add key to buffer and advance.
			putchar (c);
		}
	}
}

