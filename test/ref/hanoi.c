/*
  !!DESCRIPTION!! solves the "towers of hanoi" problem
  !!ORIGIN!!      BYTE UNIX Benchmarks
  !!LICENCE!!     Public Domain
*/

/*******************************************************************************
 *  The BYTE UNIX Benchmarks - Release 3
 *          Module: hanoi.c   SID: 3.3 5/15/91 19:30:20
 *
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Ben Smith, Rick Grehan or Tom Yager
 *	ben@bytepb.byte.com   rick_g@bytepb.byte.com   tyager@bytepb.byte.com
 *
 *******************************************************************************
 *  Modification Log:
 *  $Header: hanoi.c,v 3.5 87/08/06 08:11:14 kenj Exp $
 *  August 28, 1990 - Modified timing routines (ty)
 *
 ******************************************************************************/

#define VERBOSE
/*#define USECMDLINE*/

#include <stdio.h>
#include <stdlib.h>

unsigned short iter = 0; /* number of iterations */
char num[4];
long cnt;

int disk=5,       /* default number of disks */
    duration=10;  /* default time to run test */

void mov(unsigned char n,unsigned char f,unsigned char t)
{
char o;

	if(n == 1)
	{
		num[f]--;
		num[t]++;
	}
	else
	{
		o = (6-(f+t));
		mov(n-1,f,o);
		mov(1,f,t);
		mov(n-1,o,t);
	}

	#ifdef VERBOSE
	printf("%2d: %2d %2d %2d %2d\n",
		(int)iter,(int)num[0],(int)num[1],(int)num[2],(int)num[3]);
	#endif
}

int main(int argc,char **argv)
{
	#ifdef USECMDLINE
	if (argc < 2) {
		printf("Usage: %s [duration] [disks]\n", argv[0]);
		exit(1);
	}
	else
	{
		if(argc > 1) duration = atoi(argv[1]);
		if(argc > 2) disk = atoi(argv[2]);
	}
	#endif

	printf("towers of hanoi\ndisks: %d\n\n",disk);

	num[1] = disk;

	#ifdef VERBOSE
	printf("%2d: %2d %2d %2d %2d\n",
		(int)iter,(int)num[0],(int)num[1],(int)num[2],(int)num[3]);
	#endif

	while(num[3]<disk)
	{
		mov(disk,1,3);
		++iter;
	}

	return 0;
}
