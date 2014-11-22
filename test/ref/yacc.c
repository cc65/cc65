/*
  !!DESCRIPTION!!
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

/*#define STANDALONE*/

#include "common.h"

#ifndef YACCDBG

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
#define LEXDEBUG
#define YYDEBUG
*/

#endif

FILE *infile, *outfile;
#define getchar() fgetc(infile)

/* hack the original tables to work with both petscii and ascii */
#define CHARSETHACK

# define ID 257
# define CON 258
# define UNARYMINUS 259
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
/*#define YYMAXDEPTH 150*/
#define YYMAXDEPTH 50
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) (void)putc(c,yyout)

/* # define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar) */
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getchar())==('\n')?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)

# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;

#define yyin  infile
#define yyout outfile

extern int yylineno;
struct yysvf 
{
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;
};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

/*# define YYNEWLINE 10 */
# define YYNEWLINE ('\n')

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
yylook();
int yywrap();
yyparse();
yyerror(char *s);
yyunput(int c);
yyoutput(int c);
yyinput();
yyback(int *p,int m);
#endif

#ifdef YYDEBUG
void printchar(char *name,int ch)
{
	if((ch==YYNEWLINE))
	{
		fprintf(yyout," %s=YYNEWLINE\n",name);
	}
	else if((ch<0)||(ch>0xf0)||(!isprint(ch)))
	{
		fprintf(yyout," %s=%04x\n",name,ch &0xffff);
	}
	else
	{
		fprintf(yyout," %s='%c'\n",name,ch);
	}
}
#endif

yylex()
{
int nstr;
extern int yyprevious;

#ifdef YYDEBUG
	fprintf(yyout,"yylex()\n");
#endif

	while((nstr = yylook()) >= 0)
	{
#ifdef YYDEBUG
	fprintf(yyout,"yylex: nstr=%d\n",nstr);
#endif
yyfussy:
	switch(nstr)
	{
		case 0:
			if(yywrap()) return(0);
			break;
		case 1:
		 	return ID;
		break;
		case 2:
			return CON;
		break;
		case 3:
			;
		break;
		case 4:
			return yytext[0];
		break;
		case -1:
		break;
		default:
			fprintf(yyout,"yylex: bad switch yylook %d\n",nstr);
	}

	}
	
#ifdef YYDEBUG
	fprintf(yyout,"yylex: return 0\n");
#endif
	return(0);
}
/* end of yylex */

int yyvstop[] =
{
	0,4,0,3,4,0,2,4,0,1,4,0,2,0,1,0,0
};

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

/*
struct yysvf 
{
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;
};
*/
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
 /* 0x8d */
/* struct yywork *yytop = yycrank+141; */
 /* 0xff */
struct yywork *yytop = yycrank+255;

struct yysvf *yybgin = yysvec+1;

/*
	WARNING: this table contains one entry per character
	         in the execution character set and must match it.
*/
char yymatch[] =
{
	00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
#ifdef CHARSETHACK
	01  ,011 ,012 ,01  ,01  ,012  ,01  ,01  ,
#else
	01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
#endif
	01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
	01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
 
	011 ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
	01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
 
	'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
	'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
 
/* 0x40 (ascii) @A... (petscii) @a... */
	01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
 
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
 
/* 0x60 (ascii) @a... */
	01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
 
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
 
#ifdef CHARSETHACK
/* 0x80 */
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

/* 0xc0 (petcii) @A... */
	01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
 
	'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
	'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
 
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
 	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
#endif
	0
};
char yyextra[] =
{
	0,0,0,0,0,0,0,0,0
};

/*	ncform	4.1	83/08/11	*/

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;

unsigned char testbreak=0;

yylook()
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
/*
# ifdef LEXDEBUG
	int debug;
# endif
*/
	
	char *yylastch;
	
	/* start off machines */

/*
# ifdef LEXDEBUG
	debug = 1;
# else
	debug = 0;
# endif
*/

# ifdef LEXDEBUG
#define debug 1
# else
#define debug 0
#endif
	
#ifdef YYDEBUG
	fprintf(yyout,"yylook()\n");
# endif
	
	if (!yymorfg)
		yylastch = yytext;
	else
	{
		yymorfg=0;
		yylastch = yytext+yyleng;
	}

#ifdef YYDEBUG
	fprintf(yyout,"yylook: yymorfg=%d\n",yymorfg);
# endif
		
	for(;;)
	{
#ifdef YYDEBUG
		fprintf(yyout,"yylook:  (outer loop)");
		printchar("yyprevious",yyprevious);
# endif
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;

		testbreak=0;
		
		for (;;)
		{
# ifdef LEXDEBUG
			fprintf(yyout,"yylook:   (inner loop) state %d\n",yystate-yysvec-1);
# endif
			if(testbreak==5)
			{
				fprintf(yyout,"yylook:   error, aborted after 5 loops\n");
				exit(0);
			}
			testbreak++;
			
			yyt = yystate->yystoff;

/*			fprintf(yyout,"yylook:   yyt offs: %02x\n",yyt-yycrank); */

			
			if(yyt == yycrank)
			{		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
			}
			*yylastch++ = yych = input();

# ifdef LEXDEBUG
			fprintf(yyout,"yylook:   input ");
			printchar("yych",yych);
# endif
			
		tryagain:

# ifdef LEXDEBUG
/*			fprintf(yyout,"yylook:   yych=%02x yymatch[yych]=%02x\n",yych,yymatch[yych]); */
			fprintf(yyout,"yylook:   tryagain\n");
# endif
			yyr = yyt;

/*			fprintf(yyout,"yylook:   yyr offs: %02x\n",yyr-yycrank); */
			
			if ( yyt > yycrank)
			{
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate)
				{
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
					{
# ifdef LEXDEBUG
						fprintf(yyout,"yylook:   unput (1) ");
						printchar("*yylastch",*yylastch);
# endif
						unput(*--yylastch);
						break;
					}
					*lsp++ = yystate = yyt->advance+yysvec;
# ifdef LEXDEBUG
					fprintf(yyout,"yylook:   continue (1)\n");
# endif
					goto contin;
				}
# ifdef LEXDEBUG
				fprintf(yyout,"yylook:   ( yyt > yycrank)\n");
# endif
			}
# ifdef YYOPTIM
			else if(yyt < yycrank) /* r < yycrank */
			{		
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				fprintf(yyout,"yylook:   compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate)
				{
# ifdef LEXDEBUG
					fprintf(yyout,"yylook:   (1)\n");
# endif
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
					{
# ifdef LEXDEBUG
						fprintf(yyout,"yylook:   unput (2) ");
						printchar("*yylastch",*yylastch);
# endif
						unput(*--yylastch);
						break;
					}
					*lsp++ = yystate = yyt->advance+yysvec;
# ifdef LEXDEBUG
					fprintf(yyout,"yylook:   continue (2)\n");
# endif
					goto contin;
					
				}
# ifdef LEXDEBUG
/*
				fprintf(yyout,"yylook:   yych=%02x yymatch[yych]=%02x\n",yych,yymatch[yych]);
				fprintf(yyout,"yylook:   yyt offs: %02x\n",yyt-yycrank);
				fprintf(yyout,"yylook:   yyr offs: %02x\n",yyr-yycrank);
*/
# endif
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
/*
				fprintf(yyout,"yylook:   yyt offs: %02x <= yytop offs: %02x\n",yyt-yycrank,yytop-yycrank);
				fprintf(yyout,"yylook:   yyt->verify=%04x yysvec=%04x (yyt->verify+yysvec)=%04x == yystate=%04x\n",yyt->verify,yysvec,(yyt->verify+yysvec),yystate);
*/
				fprintf(yyout,"yylook:   try fall back character\n");
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate) 
				{
# ifdef LEXDEBUG
					fprintf(yyout,"yylook:   (2a)\n");
# endif
					
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
					{
# ifdef LEXDEBUG
/* cc65 compiles this ?!						fprintf(yyout,"yylook:   unput (3) ",); */
						fprintf(yyout,"yylook:   unput (3) ");
						printchar("*yylastch",*yylastch);
# endif
						unput(*--yylastch);
						break;
					}
					*lsp++ = yystate = yyt->advance+yysvec;
# ifdef LEXDEBUG
/*					fprintf(yyout,"yylook:   yyt offs: %02x yyt->advance=%d\n",yyt-yycrank,yyt->advance); */
					fprintf(yyout,"yylook:   continue (3)\n");
# endif
					goto contin;
					
				}
# ifdef LEXDEBUG
				fprintf(yyout,"yylook:   (2)\n");
# endif
			}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank)
			{
# ifdef LEXDEBUG
				fprintf(yyout,"yylook:   fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
			}
# endif
			else
			{
# ifdef LEXDEBUG
				fprintf(yyout,"yylook:   unput (4) ");
				printchar("*yylastch",*yylastch);
# endif
				unput(*--yylastch);
				break;
			}
		contin:
# ifdef LEXDEBUG
			fprintf(yyout,"yylook:   contin state=%d\n",yystate-yysvec-1);
# endif
			;
		}

# ifdef LEXDEBUG
		if((*(lsp-1)-yysvec-1)<0)
		{
			fprintf(yyout,"yylook:  stopped (end)\n");
		}
		else
		{
			fprintf(yyout,"yylook:  stopped at %d with\n",*(lsp-1)-yysvec-1);
		}
# endif
		while (lsp-- > yylstate)
		{
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0)
			{
				yyolsp = lsp;
				if(yyextra[*yyfnd]) /* must backup */
				{		
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate)
					{
						lsp--;
# ifdef LEXDEBUG
						fprintf(yyout,"yylook:   unput (5) ");
						printchar("*yylastch",*yylastch);
# endif
						unput(*yylastch--);
					}
				}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				fprintf(yyout,"\nyylook:  match action %d\n",*yyfnd);
				fprintf(yyout,"yylook:  done loops: %d\n",testbreak);
# endif
				return(*yyfnd++);
			}
			unput(*yylastch);
		}
		if (yytext[0] == 0  /* && feof(yyin) */)
		{
			yysptr=yysbuf;
# ifdef LEXDEBUG
			fprintf(yyout,"yylook:  done loops: %d\n",testbreak);
# endif
			return(0);
		}
		yyprevious = yytext[0] = input();

# ifdef LEXDEBUG
		fprintf(yyout,"yylook:   input ");
		printchar("yyprevious",yyprevious);
# endif

		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
/*		if(debug)putchar('\n'); */
# endif
	}

# ifdef LEXDEBUG
	fprintf(yyout,"yylook: done loops: %d\n",testbreak);
	fprintf(yyout,"yylook: return <void>\n");
# endif
}

	
yyback(p, m)
	int *p;
{
	if (p==0) return(0);
	while (*p)
	{
		if (*p++ == m)
		{
			return(1);
		}
	}
	return(0);
}
	/* the following are only used in the lex library */
yyinput()
{
	int out=input();
	
#ifdef YYDEBUG
	fprintf(yyout,"yylook:   input ");
	printchar("out",out);
#endif	
	return(out);
}
yyoutput(c)
  int c; 
{
	output(c);
}
yyunput(c)
   int c; 
{
	unput(c);
}

main() 
{
        printf("main start\n");
        infile = fopen("yacc.in","rb");
        if (infile == NULL) {
            return EXIT_FAILURE;
        }
        outfile = stdout;
        yyparse();
        fclose(infile);
        printf("main end\n");
        return 0;
}

/* yyerror - issue error message */
yyerror(s) 
char *s; 
{
        printf("[%s]\n", s);
}
short yyexca[] =
{
-1, 1,
	0, -1,
	-2, 0,
};

# define YYNPROD 15
# define YYLAST 249

short yyact[]=
{
  12,   2,   9,   8,  17,  11,  25,  17,  15,  18,
  16,  10,  18,  17,  15,   7,  16,  13,  18,   5,
   3,   1,   0,  19,  20,   0,   0,  21,  22,  23,
  24,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   6,  14,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   4,   6 
};
short yypact[]=
{
-1000,  -9,-1000,   5,  -7, -59,-1000,-1000,-1000, -40,
 -29, -40, -40,-1000,-1000, -40, -40, -40, -40, -38,
 -35, -38, -38,-1000,-1000,-1000 
};
short yypgo[]=
{
   0,  21,  20,  17,  11 
};
short yyr1[]=
{
   0,   1,   1,   1,   1,   2,   4,   4,   4,   4,
   4,   4,   4,   4,   3 
};
short yyr2[]=
{
   0,   0,   2,   3,   3,   3,   3,   3,   3,   3,
   2,   3,   1,   1,   1 
};
short yychk[]=
{
-1000,  -1,  10,  -2, 256,  -3, 257,  10,  10,  61,
  -4,  45,  40,  -3, 258,  43,  45,  42,  47,  -4,
  -4,  -4,  -4,  -4,  -4,  41 
};
short yydef[]=
{
   1,  -2,   2,   0,   0,   0,  14,   3,   4,   0,
   5,   0,   0,  12,  13,   0,   0,   0,   0,  10,
   0,   6,   7,   8,   9,  11 
};

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 1; /* 1 for debugging */
#else
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() 
{
	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */
#ifdef YYDEBUG
	 printf("yyparse: yystack\n");
#endif

#ifdef YYDEBUG
	printf("yyparse: yystate=%d, ", yystate);
	printchar("yychar",yychar);
#endif
	if( ++yyps> &yys[YYMAXDEPTH] )
	{
		yyerror( "yyparse: yacc stack overflow" );
		return(1);
	}
	*yyps = yystate;
	++yypv;
	*yypv = yyval;

 yynewstate:
#ifdef YYDEBUG
	 printf("yyparse: yynewstate\n");
#endif

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

#ifdef YYDEBUG
	 printf("yyparse: yynewstate (1)\n");
#endif
	
	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;

#ifdef YYDEBUG
	 
	printf("yyparse: yynewstate yyn=%d ",yyn);
	printchar("yychar",yychar);
#endif
	
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

#ifdef YYDEBUG
	 printf("yyparse: yynewstate (2)\n");
#endif
	
	if( yychk[ yyn=yyact[ yyn ] ] == yychar ) /* valid shift */
	{ 
		yychar = -1;
		yyval = yylval;
		yystate = yyn;

#ifdef YYDEBUG
		 printf("yyparse: yynewstate (3)\n");
#endif

		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
	}

 yydefault:
#ifdef YYDEBUG
	 printf("yyparse: yydefault yystate=%d\n",yystate);
#endif
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 )
	{
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 )
		{
			if( *yyxi == yychar ) break;
		}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
	}

#ifdef YYDEBUG
	 printf("yyparse: yyn=%d yyerrflag=%d\n",yyn,yyerrflag);
#endif
	
	if( yyn == 0 ) /* error */
	{ 
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){
		case 0:   /* brand new error */

			yyerror( "yyparse: syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   printf("yyparse: error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);

		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			printf("yyparse: error recovery discards char ");
			printchar("yychar",yychar);
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

	}

	/* reduction by production yyn */

#ifdef YYDEBUG
		printf("yyparse: reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		
		switch(yym)
		{
			case 4:
			{ 
				yyerrok; 
			}
			break;
			case 5:
			{ 
				printf("[STORE]\n");
			} 
			break;
			case 6:
			{ 
				printf("[ADD]\n");
			} 
			break;
			case 7:
			{ 
				printf("[NEG]\n[ADD]\n");
			} 
			break;
			case 8:
			{ 
				printf("[MUL]\n");
			} 
			break;
			case 9:
			{ 
				printf("[DIV]\n");
			} 
			break;
			case 10:
			{ 
				printf("[NEG]\n"); 
			} 
			break;
			case 12:
			{ 
				printf("[LOAD]\n"); 
			} 
			break;
			case 13:
			{ 	
				printf("[PUSH %s]\n", yytext);
			} 
			break;
			case 14:
			{ 
				printf("[%s]\n", yytext);
			} 
			break;
		}
		
	goto yystack;  /* stack new state and value */
}
	
int yywrap() 
{ 
	return 1; 
}
