
#pragma static-locals(1)


// original code: 35.8 sec
// 3578 bytes

// regcall: 21.8 sec
// 3584 bytes


#include <peekpoke.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#define REGCALL


void waitKey()
{
	POKE(764,255);
	while (PEEK(764)==255);
}


int
main(void)
{
	static clock_t t;
	static unsigned int count = 30000;
	static unsigned char bufferOne[5];
	static unsigned char bufferTwo[5];
	static unsigned long sec;
    static unsigned sec10;
    static unsigned int length = sizeof(bufferOne);

	clrscr ();

	 t = clock ();

	while (count != 0)
	{

#ifdef REGCALL
		MEMCPY(bufferTwo, bufferOne, length);		
		MEMCMP(bufferTwo, bufferOne, length);
#else
		memcpy(bufferTwo, bufferOne, length);		
		memcmp(bufferTwo, bufferOne, length);
#endif		
		--count;
	}

	t = clock () - t;
    
    /* Calculate stats */
    sec = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;

    /* Output stats */
    cprintf ("Total runtime: %lu.%us\n\r", sec, sec10);
	waitKey();

	return 0;
}