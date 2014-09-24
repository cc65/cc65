/*
  !!DESCRIPTION!! 
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

/*
  this test reproduces a bug that prevented the testsuites directory
  reading stuff for the c64 from working before. the bug appears to
  only occur when optimizations are enabled. it also disappears if
  the buffers inside the readdir function are declared static or
  made global.
*/

/*#define STANDALONE*/

#ifdef STANDALONE

FILE *outfile=NULL;
#define OPENTEST() outfile=stdout;
#define CLOSETEST()

#else

#endif
			 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define XNAME_MAX 16

struct Xdirent
{
	char d_name[XNAME_MAX+1];
	unsigned short d_off;
	unsigned short d_reclen;
	unsigned char  d_type;
	unsigned char  d_namlen;
};

typedef struct
{
	unsigned char fd;
	unsigned short off;
	char name[XNAME_MAX+1];
} XDIR;

unsigned char b1[4];
unsigned char b2[0x10]={"  \"test\"  "};

struct Xdirent *Xreaddir(XDIR *dir)
{
unsigned char buffer[0x40];
unsigned char temp;
unsigned char i,ii;

static struct Xdirent entry;
unsigned char fd;
static unsigned char ch;

		entry.d_off=dir->off;

		/* basic line-link / file-length */
		memcpy(buffer,b1,4);
		
		dir->off=dir->off+4;    
		entry.d_reclen=254*(buffer[2]+(buffer[3]<<8));

		/* read file entry */
		memcpy(buffer,b2,0x10);
			
		dir->off=dir->off+i;    

		printf("Xreaddir: '%s'\n",buffer);
		
		/* skip until either quote (file) or b (blocks free => end) */
		i=0;ii=0;
		while(i==0){
			temp=buffer[ii];ii++;
			if(ii>16){
				/* something went wrong...this shouldnt happen! */
				return(NULL);
			}
			else if(temp=='\"') i++;
			else if(temp=='b') {
				/* "blocks free" */
				return(NULL);
			}
		}
		printf("Xreaddir: '%s'\n",buffer);

		/* process file entry */

		i=0;  temp=buffer[ii];ii++;
		while(temp!='\"'){
			entry.d_name[i]=temp;
			i++;
			temp=buffer[ii];ii++;
		}
		entry.d_name[i]=0;
		entry.d_namlen=i;

		/* set type flag */

		return(&entry);
}

int main(void)
{
char mydirname[XNAME_MAX+1]=".";
XDIR mydir;
struct Xdirent *mydirent;
	
    printf("start\n");
    
    if((mydirent=Xreaddir(&mydir))==NULL)
    {
	    printf("NULL\n");
    }
    else
    {
	    printf("=%s\n",mydirent->d_name);
    }
    printf("done\n");

    return 0;
}
