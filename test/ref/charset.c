/*
  !!DESCRIPTION!! basic ASCII character test
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

#include <stdio.h>

#if 0

/* this kind of line-continuation for strings doesnt work properly for cc65 */

const unsigned char characters[]={
	/*0123456789abcdef0123456789abcdef*/
	/* iso646-us control-characters */
	"                                "      /* 00-1f */
	/* iso646-us printable characters */
	" !\"#$%&'()*+,-./"                     /* 20-2f !"#$%&'()*+,-./ */
	"0123456789"                            /* 30-39 0123456789      */
	":;<=>?@"                               /* 3a-40 :;<=>?@         */
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"            /* 41-5a A-Z             */
	"[\\]^_`"                               /* 5b-60 [\]^_`          */
	"abcdefghijklmnopqrstuvwxyz"            /* 61-7a a-z             */
	"{|}~ "                                 /* 7b-7f {|}~            */
	/* iso8859-15 extended characters */
};

#endif

const unsigned char characters[]={
	/*0123456789abcdef0123456789abcdef*/
	/* iso646-us control-characters */
	/* 00-1f */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	/* iso646-us printable characters */
	/* 20-2f !"#$%&'()*+,-./ */
	' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
	/* 30-39 0123456789      */
	'0','1','2','3','4','5','6','7','8','9',
	/* 3a-40 :;<=>?@         */
	':',';','<','=','>','?','@',
	/* 41-5a A-Z             */
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	/* 5b-60 [\]^_`          */
	'[','\\',']','^','_','`',
	/* 61-7a a-z             */
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	/* 7b-7f {|}~            */
	'{','|','}','~',' '
	/* iso8859-15 extended characters */
};

void printchars(unsigned char a,unsigned char b){
	for(b++;a!=b;a++)
/*                printf("%02x ",a); */
/*                printf("%02x ",characters[a]); */
		printf("%c",characters[a]);
	printf("\n");
}

int main(void) {
	printf("characters:\n\n");
	printchars(0x61,0x7a);
	printchars(0x41,0x5a);
	printf("numbers:\n\n");
	printchars(0x30,0x39);
	printf("other:\n\n");
	printchars(0x20,0x2f);
	/*printchars(0x3a,0x40);*/
	printchars(0x3a,0x3f);
	/*printchars(0x5b,0x60);*/
	/*printchars(0x7b,0x7f);*/
	printf("\n\n");
	printf("slash:               '%c'\n",'/');
	printf("backslash:           '%c'\n",'\\');
	printf("curly braces open:   '%c'\n",'{');
	printf("curly braces close:  '%c'\n",'}');
	printf("square braces open:  '%c'\n",'[');
	printf("square braces close: '%c'\n",']');
	printf("underscore:          '%c'\n",'_');
	printf("tilde:               '%c'\n",'~');
	printf("pipe:                '%c'\n",'|');
	printf("apostroph:           '%c'\n",'\'');
	printf("single quote         '%c'\n",'`');
	printf("xor                  '%c'\n",'^');
	printf("at                   '%c'\n",'@');

	return 0;
}
