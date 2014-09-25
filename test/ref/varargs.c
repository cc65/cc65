/*
  !!DESCRIPTION!! varargs test
  !!ORIGIN!!
  !!LICENCE!!     public domain
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void chk0(char *format,...);
void chk1(int fd,char *format,...);

#if 0
// old workaround for broken varargs

void chk0(char *format,...){
    __asm__ ("pha");    // save argument size
    {
//va_list ap;
char *ap;
char *_format;
static char string[0x100];

//    va_start(ap,format);
    __asm__ ("pla");       // restore argument size
    __asm__ ("ldx #$00");  // clear hibyte of AX
    ap=__AX__;
    ap+=(char*)&format;
    // get value of format
    ap-=2;
    _format=*((char**)ap);

//    vsprintf(string,format,ap);
    vsprintf(&string[0],_format,ap);
    printf("format:%s,string:%s\n",_format,string);
//    va_end(ap);

    }
}

void chk1(int fd,char *format,...){
    __asm__ ("pha");    // save argument size
    {
//va_list ap;
char *ap;
char *_format;
int _fd;
static char string[0x100];

//    va_start(ap,format);
    __asm__ ("pla");       // restore argument size
    __asm__ ("ldx #$00");  // clear hibyte of AX
    ap=__AX__;
    ap+=(char*)&format;
    // get value of fd
    ap-=2;
    _fd=*((int*)ap);
    // get value of format
    ap-=2;
    _format=*((char**)ap);

//    vsprintf(string,format,ap);
    vsprintf(&string[0],_format,ap);
    printf("fd:%d,format:%s,string:%s\n",_fd,_format,string);
//    va_end(ap);

    }
}

#endif

void chk0(char *format,...){
va_list ap;
static char string[0x100];
    va_start(ap,format);
    vsprintf(string,format,ap);
    printf("format:%s,string:%s\n",format,string);
    va_end(ap);
}

void chk1(int fd,char *format,...){
va_list ap;
static char string[0x100];

    va_start(ap,format);

    vsprintf(string,format,ap);
	printf("fd:%d,format:%s,string:%s\n",fd,format,string);
    va_end(ap);
}

int main(int argc,char **argv) {
    printf("varargs test\n");

    printf("\nchk0/0:\n");chk0("chk0 %s","arg0");
    printf("\nchk0/1:\n");chk0("chk0 %s %s","arg0","arg1");
    printf("\nchk0/2:\n");chk0("chk0 %s %s %s","arg0","arg1","arg2");

    printf("\nchk1/0:\n");chk1(0xfd,"chk1 %s","arg0");
    printf("\nchk1/1:\n");chk1(0xfd,"chk1 %s %s","arg0","arg1");
    printf("\nchk1/2:\n");chk1(0xfd,"chk1 %s %s %s","arg0","arg1","arg2");

    return 0;
}
