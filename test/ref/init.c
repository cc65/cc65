/*
  !!DESCRIPTION!! variable initialization
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"
/* todo: add back conditional stuff here ! */

typedef struct { int codes[3]; char name[6]; } Word;

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES

#ifdef NO_OLD_FUNC_DECL
f();
void g(Word *p);
h();
#else
f();
g();
h();
#endif

#endif

/*
Word words[] = {
        1, 2, 3,"if",
	{ { 4, 5 }, { 'f', 'o', 'r' } },
        6, 7, 8, {"else"},
        { { 9, 10, 11,}, 'w', 'h', 'i', 'l', 'e', },
        { 0 },
}, *wordlist = words;
*/

Word words[] = {
        {{1, 2, 3},"if"},
	{ { 4, 5 }, { 'f', 'o', 'r' } },
        {{6, 7, 8}, "else"},
        { { 9, 10, 11}, {'w', 'h', 'i', 'l', 'e', }},
        {{ 0 }},
}, *wordlist = words;

/*int x[][5] = { 1, 2, 3, 4, 0, { 5, 6 }, { 7 } };*/
int x[][5] = { {1, 2, 3, 4, 0 }, { 5, 6 }, { 7 } };
int *y[] = { x[0], x[1], x[2], 0 };

main()
{
	int i, j;

	for (i = 0; y[i]; i++) {
		for (j = 0; y[i][j]; j++)
			printf(" %d", y[i][j]);
		printf("\n");
	}
	f();
	g(wordlist);
	return 0;
}

f() {
	static char *keywords[] = {"if", "for", "else", "while", 0, };
	char **p;

	for (p = keywords; *p; p++)
		printf("%s\n", *p);
}

#ifdef NO_OLD_FUNC_DECL
void g(Word *p)
#else
g(p)
Word *p;
#endif
{
	int i;

	for ( ; p->codes[0]; p++) {
		for (i = 0; i < sizeof p->codes/sizeof(p->codes[0]); i++)
			printf("%d ", p->codes[i]);
		printf("%s\n", p->name);
	}
	h();
}

h()
{
	int i;

	for (i = 0; i < sizeof(words)/sizeof(Word); i++)
		printf("%d %d %d %s\n", words[i].codes[0],
			words[i].codes[1], words[i].codes[2],
			&words[i].name[0]);
}
