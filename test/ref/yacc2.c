/*
  !!DESCRIPTION!!
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

#include <stdio.h>

# define YYTYPE char
struct yywork 
{ 
	YYTYPE verify, advance; 
} yycrank[] =
{
	{0,0},	{0,0},	{1,3},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{1,4},	{1,3},
	{0,0},	{0,0},	{0,0},	{0,0},

	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
 
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
 
	{0,0},	{1,5},	{5,7},	{5,7},
	{5,7},	{5,7},	{5,7},	{5,7},
	{5,7},	{5,7},	{5,7},	{5,7},
	{0,0},	{0,0},	{0,0},	{0,0},
/* 0x40 */
	{0,0},	{0,0},	{1,6},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{0,0},	{0,0},	{0,0},
 
	{0,0},	{0,0},	{0,0},	{0,0},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
 
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{0,0},	{0,0},
 
	{0,0},	{0,0},	{6,8},	{0,0},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
/* 0x80 */
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{0,0},	{0,0},

#ifdef CHARSETHACK
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},

 	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},

 	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},
	{0,0},	{0,0},	{0,0},	{0,0},

/* 0xc0 */
	{0,0},	{0,0},	{1,6},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{6,8},	{6,8},	{6,8},
	{6,8},	{0,0},	{0,0},	{0,0},
#endif
	{0,0}
};

struct yywork *yytop = yycrank+255;

int yyvstop[] =
{
	0,4,0,3,4,0,2,4,0,1,4,0,2,0,1,0,0
};

struct yysvf 
{
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;
};

struct yysvf yysvec[] =
{
	{0,	0,	0},
	{yycrank+-1,	0,		0},
	{yycrank+0,	yysvec+1,	0},
	{yycrank+0,	0,		yyvstop+1},
	{yycrank+0,	0,		yyvstop+3},
	{yycrank+2,	0,		yyvstop+6},
	{yycrank+19,	0,		yyvstop+9},
	{yycrank+0,	yysvec+5,	yyvstop+12},
	{yycrank+0,	yysvec+6,	yyvstop+14},
	{0,	0,	0}
};

#if 0
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):GETCHAR())==('\n')?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
// *yylastch++ = yych = input();
void subtest1(void)
{
	*yylastch++ = yych = input();
}
#endif

// do some bogus operation to destroy all registers etc
static int bog=1234;
#if 0
void bogus(void)
{
	bog*=0x1234;
}
#else
#define bogus()	bog+=0x1234
#endif

#if 1
// yyt = yyt + yych;
void subtest2(void)
{
	register struct yywork *yyt;
	int yych;

	yyt=yycrank;
	yych=10;

	bogus();
	yyt = yyt + yych;

	printf("yyt: %d %d\n",yyt->verify,yyt->advance);
}
#endif

#if 1
// if(yyt <= yytop && yyt->verify+yysvec == yystate)
void subtest3(void)
{
	register struct yywork *yyt;
	register struct yysvf *yystate;

	yyt=yycrank;
	yystate=yysvec;
	
	bogus();
	if(yyt <= yytop && yyt->verify+yysvec == yystate)
	{
		printf("if ok %d %d\n",yyt->verify,yyt->advance);
	}
	else
	{
		printf("if not ok %d %d\n",yyt->verify,yyt->advance);
	}
}
#endif

short yyr2[]=
{
   0,   0,   2,   3,   3,   3,   3,   3,   3,   3,
   2,   3,   1,   1,   1 
};

// yyps -= yyr2[yyn];
void subtest4(void)
{
	register short *yyps, yyn;

	yyps=0x8004;
	yyn=0;

	while(yyn<14)
	{
		bogus();
		yyps -= yyr2[yyn];

		yyn++;
	}
	printf("yyps: %04x\n",yyps);
}

#if 1

int yylookret=10;
yylook()
{
	yylookret--;
	return yylookret;
}

// while((nstr = yylook()) >= 0)
void subtest5(void)
{
	int nstr;

	bogus();
	while((nstr = yylook()) >= 0)
	{
		printf("nstr: %04x\n",nstr);
		bogus();
	}
}
#endif

int main(void)
{
//    subtest1();
    subtest2();
    subtest3();
    subtest4();
    subtest5();

    return 0;
}
