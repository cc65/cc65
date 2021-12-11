/*
** simple file I/O test
**
** 12-Jun-2000, Christian Groessler
**
** please compile with
**   cl65 -tsystem ft.c getsp.s -o ft.com
**
** The program asks for a filename (if it hasn't
** got one from argv). I then opens the file,
** reads the the first 16 bytes and displays them
** (as hex values).
** The values of sp (cc65 runtime stack pointer)
** are displayed at some places. The displayed
** value should always be the same.
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <conio.h>
#include <unistd.h>

extern int getsp(void);  /* is provided in getsp.s */

/* Atari's fd indirection table */
#ifdef __ATARI__
extern char __fd_index[];
struct fd_t {
  char usage;
  char iocb;
  char dev;
  char flag;
};
extern struct fd_t __fd_table[];
#endif

int main(int argc,char **argv)
{
    char *filename,*x;
    char buf[20];
    int i,l,lr;
    int fd;
    int csp;

    if (argc >= 2) {
        filename = *(argv+1);
    }
    else {
        printf("\nfilename: ");
        x = fgets(buf,19,stdin);
        printf("\n");
        if (!x) {
            printf("nothing read\n");
            return(0);
        }
#if 0
        l = strlen(x);
        printf("read: ");
        for (i=0; i<l; i++) printf("%02X ",*(x+i)); printf("\n");
#endif
        filename = x;
    }
    printf("using filename \"%s\"\n",filename);
    csp = getsp();
    printf("now opening file... sp = %d\n",csp);
    fd = open(filename,O_RDONLY);
    csp = getsp();
    if (fd == -1) {
        char x1 = _oserror;
        printf("open failed: os: %d,\n\terrno: %d, sp = %d\n",x1,errno,csp);
        cgetc();
        return(0);
    }
    printf("open success -- handle = $%x, sp = %d\n",fd,csp);
#ifdef __ATARI__
    printf("fd_index:\n ");
    for (i=0; i<12; i++) printf("%02X ",__fd_index[i]);
    printf("\nfd_table:\n");
    for (i=0; i<8; i++) {
        printf(" usa: %d, iocb: %02X, dev: %02X\n",
               __fd_table[i].usage,
               __fd_table[i].iocb,
               __fd_table[i].dev);
    }
#endif
    lr = read(fd,buf,16);  /* read first 16 bytes */
    csp = getsp();
    if (lr == -1) {
        printf("read failed: %d (sp = %d)\n",errno,csp);
        cgetc();
        return(0);
    }
    l = close(fd);
    if (l == -1) {
        printf("close failed: %d\n",errno);
        cgetc();
        return(0);
    }
    csp = getsp();
    printf("\n\nThe data read: (%d bytes, sp = %d)\n",lr,csp);
    for (i=0; i<lr; i++) {
        printf("%02X ",buf[i]);
        if (!((i+1) & 7)) printf("\n");
    }
    printf("\n\npress return to exit...");
    getchar();
    return(0);
}
