/*

    a2tools - utilities for transferring data between Unix and Apple II
              DOS 3.3 disk images.

    Copyright (C) 1998, 2001 Terry Kyriacopoulos

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Author's e-mail address: terryk@echo-on.net

    -------------------------------------------------------------------

    Cleaned up abuse of C semantics to compile clean
    Replaced original line #820 (now line #831)
        while(i<30 && a2_name[i]) padded_name[i]=a2_name[i++] & 0x7f;
    With:
        while(i<30 && a2_name[i]) { padded_name[i]=a2_name[i] & 0x7f; i++; }
    Michael Pohoreski, Jan 16, 2016

    -------------------------------------------------------------------

    Modified to be more portable: Unix specifics are marked as such.
    ANSI-C is assumed, code is now acceptable to C++ as well,
    type definitions are straighetend up, unused variables are removed,
    casts are added when required by C++.

    Paul Schlyter, 2001-03-20,  pausch@saaf.se

    -------------------------------------------------------------------

    Improvements to accomodate MS-DOS have been made:

	- code fixed to work properly on a 16-bit platform
	- conditional compilation used to select OS-specific code
	  automatically
	- user interface is now more OS-specific:
		- argv[0] command selection for UNIX, argv[1] for DOS
		- stdin/stdout forbidden on binary data in DOS
	- optional source/destination pathnames for in/out commands
	- improved documentation

    Terry Kyriacopoulos, April 8, 2001    terryk@echo-on.net


*/

#ifndef UNIX
  #ifdef __unix__
    #define UNIX
  #endif
#endif
#ifndef DOS
  #ifdef __MSDOS__
    #define DOS
  #endif
#endif

#ifndef UNIX
  #ifndef DOS
    #error  Please define macro UNIX or DOS.
  #endif
#else
  #ifdef DOS
    #error  Both macros UNIX and DOS are defined!
  #endif
#endif

#ifdef DOS
const char *const DOS_HelpText =

"\n"
"a2tools - utility for transferring files from/to Apple II .dsk images\n"
"          Copyright (C) 1998, 2001  Terry Kyriacopoulos\n"
"          Copyright (C) 2001 Paul Schlyter\n"
"          Copyright (C) 2016 Michael Pohoreski\n"
"\n"
"    Usage:\n"
"\n"
"	a2 dir <dsk_image>\n"
"	a2 out [-r] <dsk_image> <a2_name> [<dest_file>]\n"
"	a2 in [-r] <type>[.<hex_addr>] <dsk_image> <a2_name> [<source>]\n"
"	a2 del <dsk_image> <a2_name>\n"
"\n"
"	-r (raw mode):  Suppress all filetype-dependent processing\n"
"			and copy everything as-is.\n"
"\n"
"	<type>: one of t,i,a,b,s,r,x,y (do not use -)\n"
"	<hex_addr>: base address in hex, for type B (binary)\n"
"\n"
"	Quotes may be used around names with spaces, use \\\"\n"
"	to include a quote in the name.\n"
;
#endif

/* Apple Integer and AppleSoft BASIC tokens. */

const char *const Integer_tokens[] = {

" HIMEM:",	"",		" _ ",		":",
" LOAD ",	" SAVE ",	" CON ",	" RUN ",
" RUN ",	" DEL ",	",",		" NEW ",
" CLR ",	" AUTO ",	",",		" MAN ",
" HIMEM:",	" LOMEM:",	"+",		"-",
"*",		"/",		"=",		"#",
">=",		">",		"<=",		"<>",
"<",		" AND ",	" OR ",		" MOD ",
" ^ ",		"+",		"(",		",",
" THEN ",	" THEN ",	",",		",",
"\"",		"\"",		"(",		"!",
"!",		"(",		" PEEK ",	" RND ",
" SGN ",	" ABS ",	" PDL ",	" RNDX ",
"(",		"+",		"-",		" NOT ",
"(",		"=",		"#",		" LEN(",
" ASC(",	" SCRN(",	",",		"(",

"$",		"$",		"(",		",",
",",		";",		";",		";",
",",		",",		",",		" TEXT ",
" GR ",		" CALL ",	" DIM ",	" DIM ",
" TAB ",	" END ",	" INPUT ",	" INPUT ",
" INPUT ",	" FOR ",	"=",		" TO ",
" STEP ",	" NEXT ",	",",		" RETURN ",
" GOSUB ",	" REM ",	" LET ",	" GOTO ",
" IF ",		" PRINT ",	" PRINT ",	" PRINT ",
" POKE ",	",",		" COLOR=",	" PLOT ",
",",		" HLIN ",	",",		" AT ",
" VLIN ",	",",		" AT ",		" VTAB ",
"=",		"=",		")",		")",
" LIST ",	",",		" LIST ",	" POP ",
" NODSP ",	" NODSP ",	" NOTRACE ",	" DSP ",
" DSP ",	" TRACE ",	" PR#",		" IN#"

};


const char *const Applesoft_tokens[] = {

" END ",	" FOR ",	" NEXT ",	" DATA ",
" INPUT ",	" DEL ",	" DIM ",	" READ ",
" GR ",		" TEXT ",	" PR#",		" IN#",
" CALL ",	" PLOT ",	" HLIN ",	" VLIN ",
" HGR2 ",	" HGR ",	" HCOLOR=",	" HPLOT ",
" DRAW ",	" XDRAW ",	" HTAB ",	" HOME ",
" ROT=",	" SCALE=",	" SHLOAD ",	" TRACE ",
" NOTRACE ",	" NORMAL ",	" INVERSE ",	" FLASH ",
" COLOR=",	" POP ",	" VTAB ",	" HIMEM:",
" LOMEM:",	" ONERR ",	" RESUME ",	" RECALL ",
" STORE ",	" SPEED=",	" LET ",	" GOTO ",
" RUN ",	" IF ",		" RESTORE ",	" & ",
" GOSUB ",	" RETURN ",	" REM ",	" STOP ",
" ON ",		" WAIT ",	" LOAD ",	" SAVE ",
" DEF ",	" POKE ",	" PRINT ",	" CONT ",
" LIST ",	" CLEAR ",	" GET ",	" NEW ",

" TAB(",	" TO ",		" FN ",		" SPC(",
" THEN ",	" AT ",		" NOT ",	" STEP ",
" + ",		" - ",		" * ",		" / ",
" ^ ",		" AND ",	" OR ",		" > ",
" = ",		" < ",		" SGN ",	" INT ",
" ABS ",	" USR ",	" FRE ",	" SCRN(",
" PDL ",	" POS ",	" SQR ",	" RND ",
" LOG ",	" EXP ",	" COS ",	" SIN ",
" TAN ",	" ATN ",	" PEEK ",	" LEN ",
" STR$ ",	" VAL ",	" ASC ",	" CHR$ ",
" LEFT$ ",	" RIGHT$ ",	" MID$ ",	"  ",

" SYNTAX ",			" RETURN WITHOUT GOSUB ",
" OUT OF DATA ",		" ILLEGAL QUANTITY ",
" OVERFLOW ",			" OUT OF MEMORY ",
" UNDEF'D STATEMENT ",		" BAD SUBSCRIPT ",
" REDIM'D ARRAY ",		" DIVISION BY ZERO ",
" ILLEGAL DIRECT ",		" TYPE MISMATCH ",
" STRING TOO LONG ",		" FORMULA TOO COMPLEX ",
" CAN'T CONTINUE ",		" UNDEF'D FUNCTION ",

" ERROR \a",	"",		"",		""

};

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define FILETYPE_T 0x00
#define FILETYPE_I 0x01
#define FILETYPE_A 0x02
#define FILETYPE_B 0x04
#define FILETYPE_S 0x08
#define FILETYPE_R 0x10
#define FILETYPE_X 0x20
#define FILETYPE_Y 0x40
/* X - "new A", Y - "new B" */

#define MAX_HOPS  560

#define VTOC_CHK_NO 6
const unsigned char vtoc_chk_offset[VTOC_CHK_NO] =
	{ 0x03, 0x27, 0x34, 0x35, 0x36, 0x37};
const unsigned char vtoc_chk_value[VTOC_CHK_NO] =
	{ 0x03, 0x7a, 0x23, 0x10, 0x00, 0x01};

FILE *from_file=NULL, *to_file=NULL, *image_fp=NULL;
char *extfilename, *extfilemode;

unsigned char padded_name[30], dir_entry_data[35];
unsigned char vtocbuffer[256];
unsigned int begun, baseaddress, rawmode, filetype, new_sectors;
unsigned long dir_entry_pos;

void quit(int exitcode, const char *exitmsg) { // const
    fprintf(stderr,"%s",exitmsg);
    if (image_fp) fclose(image_fp);
    if (from_file) fclose(from_file);
    if (to_file) fclose(to_file);
    exit(exitcode);
    }

int seek_sect (unsigned int track, unsigned int sector) {
    if (track >= 35 || sector >= 16)
	quit(1,"seek on .dsk out of range.\n");
    return fseek(image_fp, (track*16uL+sector)*256, SEEK_SET);
    }

void read_sect (int track, int sector, unsigned char buffer[256]) {
    int i;
    seek_sect(track, sector);
    for (i=0; i<256; i++) buffer[i]=fgetc(image_fp);
    }

void write_sect (int track, int sector, unsigned char buffer[256]) {
    int i;
    seek_sect(track, sector);
    for (i=0; i<256; i++) fputc(buffer[i],image_fp);
    }

int dir_do (int (*what_to_do)(unsigned char *) ) {
    unsigned char buffer[256];
    unsigned int cur_trk, cur_sec, i, found, hop;
    hop=found=0;
    buffer[1]=vtocbuffer[1];
    buffer[2]=vtocbuffer[2];
    while(++hop < MAX_HOPS && !found && (buffer[1] || buffer[2])) {
	cur_trk=buffer[1];
	cur_sec=buffer[2];
	read_sect (buffer[1],buffer[2],buffer);
	i=0x0b;
	while(i<=0xdd && !(found=(*what_to_do)(&buffer[i]))) i+=35;
	if (found) dir_entry_pos=(cur_trk*16uL+cur_sec)*256+i;
	}
    if (hop >= MAX_HOPS) quit(2,"\n***Corrupted directory\n\n");
    return found;
    }

int dir_find_name(unsigned char *buffer) {
    int j;
    j=0;
    if (buffer[0] == 0xff || buffer[3] == 0) return 0;
    while(j<30 && padded_name[j]==(buffer[j+3] & 0x7f)) j++;
    if (j != 30) return 0;
    for (j=0; j<35; j++) dir_entry_data[j]=buffer[j];
    return 1;
    }

int dir_find_space(unsigned char *buffer) {
    return (buffer[0] == 0xff || buffer[3] ==0);
    }

int dir_print_entry(unsigned char *buffer) {
    int j;
    if (buffer[0]!=0xff && buffer[3]!=0) {
	/* entry is present */
	printf(" ");
	if (buffer[2] & 0x80) printf("*"); else printf(" ");
	switch(buffer[2] & 0x7f) {
	    case FILETYPE_T : printf("T"); break; 
	    case FILETYPE_I : printf("I"); break;
	    case FILETYPE_A : printf("A"); break; 
	    case FILETYPE_B : printf("B"); break; 
	    case FILETYPE_S : printf("S"); break;
	    case FILETYPE_R : printf("R"); break; 
	    case FILETYPE_X : printf("X"); break; 
	    case FILETYPE_Y : printf("Y"); break; 
	    default : printf("?");
	    }
	printf(" %03u ",buffer[33]+buffer[34]*256u);
	for (j=3; j<33; j++)
	    printf("%c",(buffer[j] & 0x7f));
	printf("\n");
	}
    return 0;
    }

int preproc (int procmode) {
    /* procmode: 0 - raw, 1 - text, 2 -binary */
    static unsigned long bytepos, lengthspec_pos;
    static int c;
    unsigned int sect_pos;
    sect_pos=0;
    if (!begun) {
	begun = 1;
	bytepos = 0;
	c=fgetc(from_file);
	if (procmode == 2) {
	    fputc((baseaddress & 0xff),image_fp);
	    fputc((baseaddress >> 8),image_fp);
	    /* we don't know the length now, so save the spot in the image */
	    lengthspec_pos=ftell(image_fp);
	    fputc(0xff,image_fp);
	    fputc(0xff,image_fp);
	    sect_pos = 4;
	    }
	}
    while (c != EOF && sect_pos < 256) {
	if (procmode == 1) {
	    if ((c & 0x7f) == '\n') c = '\r';
	    c |= 0x80;
	    }
	fputc(c,image_fp);
	c=fgetc(from_file);
	sect_pos++;
	bytepos++;
	}
    while (sect_pos++ < 256) fputc(0,image_fp);
    if (c == EOF && procmode == 2) {
	/* now we know the length */
	fseek(image_fp, lengthspec_pos, SEEK_SET);
	fputc((bytepos & 0xff),image_fp);
	fputc((bytepos >> 8),image_fp);
	}
    return (c == EOF);
    }

void new_sector(unsigned int *track, unsigned int *sector) {
    /* find a free sector, quit if no more */
    unsigned int byteoffset, bitmask;
    int lasttrack, cur_track, cur_sector, direction;
    /* force sane values, in case vtoc contains garbage */
    if (vtocbuffer[0x31]==1) direction=1; else direction=-1;
    cur_track=lasttrack=vtocbuffer[0x30] % 35u;
    cur_sector=15;
    for (;;) {
        byteoffset=0x39+(cur_track<<2)-(cur_sector>>3&1);
	bitmask=(1 <<(cur_sector & 0x07));
	if (vtocbuffer[byteoffset] & bitmask) {
	    vtocbuffer[byteoffset]&=0xff^bitmask;
	    break;
	    }
	else if (!cur_sector--) {
	    cur_sector=15;
	    cur_track+=direction;
	    if (cur_track >= 35) {
		cur_track=17;
		direction=-1;
		}
	    else if (cur_track < 0) {
		cur_track=18;
		direction=1;
		}
	    if (cur_track==lasttrack) quit(3,"Disk Full.\n");
	    }
	}
    *track=vtocbuffer[0x30]=cur_track;
    *sector=cur_sector;
    vtocbuffer[0x31]=direction % 256u;
    new_sectors++;
    }

void free_sector(int track, int sector) {
    vtocbuffer[0x39+(track<<2)-(sector>>3&1)]|=1<<(sector&0x07);
    }

void postproc_B (void) {
    static unsigned int filelength, bytepos;
    unsigned int sect_pos;
    sect_pos=0;
    if (!begun) {
	begun = 1;
	bytepos = 0;
	fgetc(image_fp); /* Ignore 2 byte base address */
	fgetc(image_fp);
	filelength= fgetc(image_fp) + (fgetc(image_fp) * 256u);
	sect_pos = 4;
	}
    while (bytepos < filelength && sect_pos < 256) {
	fputc(fgetc(image_fp),to_file);
	sect_pos++;
	bytepos++;
	}
    }

void postproc_A (void) {
    static unsigned int bufstat, tokens_left, lastspot;
    static unsigned char lineheader[4];
    unsigned int sect_pos, c;
    sect_pos=0;
    if (!begun) { /* first sector, initialize */
	begun = 1;
	fgetc(image_fp); /* ignore the length data, we use */
	fgetc(image_fp); /* null line pointer as EOF	   */
	sect_pos = 2;
	lastspot = 0x0801; /* normal absolute beginning address */
	tokens_left = bufstat = 0;
	}
    while(lastspot && sect_pos < 256) {
	if (!tokens_left && !bufstat) bufstat = 4;
	while (bufstat > 0 && sect_pos < 256) {
	    lineheader[4-bufstat]=fgetc(image_fp);
	    sect_pos++;
	    bufstat--;
	    }
	if (!tokens_left && !bufstat &&
	    (lastspot=lineheader[0]+lineheader[1]*256u)) {
	    tokens_left = 1;
	    fprintf(to_file,"\n");
	    fprintf(to_file," %u ",lineheader[2]+lineheader[3]*256u);
	    }
	while (tokens_left && lastspot && sect_pos < 256) {
	    if ((tokens_left=c=fgetc(image_fp)) & 0x80)
		fprintf(to_file,"%s",Applesoft_tokens[(c & 0x7f)]);
	    else if (c) fprintf(to_file,"%c",c);
	    sect_pos++;
	    }
	}
    if (!lastspot) fprintf(to_file,"\n\n");
    }

void postproc_I (void) {
    static unsigned int filelength, bytepos;
    static unsigned int bufstat, inputmode, quotemode, varmode;
    static unsigned char numbuf[3];
    unsigned int sect_pos, c;
    sect_pos=0;
    if (!begun) { /* first sector, initialize */
	begun = 1;
	filelength = fgetc(image_fp) + (fgetc(image_fp) * 256u);
	sect_pos = 2;
	bytepos = inputmode = bufstat = quotemode = varmode = 0;
	}
    /* inputmode: 0 - header, 1 - integer, 2 - tokens */
    /* varmode: 1 means we are in the middle of an identifier */
    while(bytepos < filelength && sect_pos < 256) {
	if (inputmode < 2 && !bufstat) bufstat = 3 - inputmode;
	while (bufstat > 0 && bytepos < filelength && sect_pos < 256) {
	    numbuf[3-bufstat]=fgetc(image_fp);
	    sect_pos++;
	    bytepos++;
	    bufstat--;
	    }
	if (!bufstat && inputmode == 0) {
	    fprintf(to_file,"\n");
	    fprintf(to_file,"%5u ",numbuf[1]+(numbuf[2]*256u));
	    inputmode = 2;
	    }
	if (!bufstat && inputmode == 1) {
	    fprintf(to_file,"%u",numbuf[1]+(numbuf[2]*256u));
	    inputmode = 2;
	    }
	while (inputmode == 2 && bytepos < filelength && sect_pos < 256) {
	    c=fgetc(image_fp);
	    sect_pos++;
	    bytepos++;
	    /* 0x28: open quote, 0x29: close quote, 0x5d: REM token */
	    if (c == 0x28 || c == 0x5d) quotemode = 1;
	    if (c == 0x29) quotemode = 0;
	    /* Look for integer, unless in comment, string, or identifier */
	    if (!quotemode && !varmode && c >= 0xb0 && c <= 0xb9)
		inputmode = 1;
	    else {
		/* Identifiers begin with letter, may contain digit */
		varmode = (c >= 0xc1 && c <= 0xda) ||
			 ((c >= 0xb0 && c <= 0xb9) && varmode);
		if (c == 0x01) inputmode = quotemode = 0;
		else if (c & 0x80) fprintf(to_file,"%c",(c & 0x7f));
		else fprintf(to_file,"%s",Integer_tokens[c]);
		}
	    }
	}
    if (bytepos >= filelength) fprintf(to_file,"\n\n");
    }

void postproc_T (void) {
    static unsigned int not_eof;
    unsigned int sect_pos, c;
    sect_pos=0;
    if (!begun) begun = not_eof = 1;
    while (not_eof && sect_pos < 256 &&
	(not_eof=c=fgetc(image_fp))) {
	c &= 0x7f;
	if (c == '\r') c='\n';
	fputc(c,to_file);
	sect_pos++;
	}
    }

void postproc_raw (void) {
    unsigned int sect_pos;
    for (sect_pos=0; sect_pos < 256; sect_pos++)
	fputc(fgetc(image_fp),to_file);
    }

void a2ls (void) {
    unsigned int trkmap, free_sect, i, j;
    free_sect = 0;

    /* count the free sectors */
    for (i=0x38; i<=0xc0; i+=4) {
	trkmap=vtocbuffer[i]*256u + vtocbuffer[i+1];
	for (j=0; j<16; j++) free_sect += ((trkmap & (1<<j))!=0);
	}
    printf("\nDisk Volume %u, Free Blocks: %u\n\n",
	vtocbuffer[0x06],free_sect);
    dir_do(dir_print_entry);
    printf("\n");
    }

void a2rm (void) {
    unsigned char listbuffer[256];
    unsigned int hop, next_trk, next_sec, i;
    if (!dir_do(dir_find_name)) quit(4,"File not found.\n");
    hop = begun = 0;
    next_trk=dir_entry_data[0];
    next_sec=dir_entry_data[1];
    fseek(image_fp, dir_entry_pos, SEEK_SET);
    fputc(0xff,image_fp); /* mark as deleted */
    while(++hop < MAX_HOPS && (next_trk || next_sec)) {
	read_sect(next_trk, next_sec, listbuffer);
	free_sector(next_trk, next_sec);
	next_trk=listbuffer[1];
	next_sec=listbuffer[2];
	for (i=0x0c; i <=0xfe; i+=2)
	    if (listbuffer[i] || listbuffer[i+1])
		free_sector(listbuffer[i],listbuffer[i+1]);
	}
    if (hop >= MAX_HOPS) quit(5,"Corrupted sector list\n\n");
    write_sect(0x11, 0, vtocbuffer);
    }

void a2out (void) {
    unsigned char listbuffer[256];
    unsigned int hop, next_trk, next_sec, i, j;
    void (*postproc_function)(void);
    if (!dir_do(dir_find_name)) quit(6,"File not found.\n");
    hop = begun = 0;
    next_trk=dir_entry_data[0];
    next_sec=dir_entry_data[1];
    filetype=(dir_entry_data[2] & 0x7f);

    if      (filetype == FILETYPE_T) postproc_function= postproc_T;
    else if (filetype == FILETYPE_B) postproc_function= postproc_B;
    else if (filetype == FILETYPE_A) postproc_function= postproc_A;
    else if (filetype == FILETYPE_I) postproc_function= postproc_I;
    else if (!rawmode)
	quit(7,"File type supported in raw mode only.\n");
    if (rawmode) postproc_function= postproc_raw;

#ifdef DOS
    extfilemode="w";
    if (rawmode || filetype == FILETYPE_B) {
	extfilemode="wb";
	if (to_file)
	    quit(8,"stdout not allowed for binary output.\n");
	}
#else
    extfilemode="w";
#endif

    if (!to_file && !(to_file=fopen(extfilename,extfilemode))) {
	perror(extfilename);
	quit(9,"");
	}

    while(++hop < MAX_HOPS && (next_trk || next_sec)) {
	read_sect(next_trk, next_sec, listbuffer);
	next_trk=listbuffer[1];
	next_sec=listbuffer[2];
	for (i=0x0c; i <= 0xfe; i+=2)
	    if (!listbuffer[i] && !listbuffer[i+1]) {
		if (filetype != FILETYPE_T || !rawmode) {
		    next_trk=next_sec=0;
		    break;
		    }
		else for (j=0; j<256; j++) fputc(0,to_file);
		}
	    else {
		++hop;
		seek_sect(listbuffer[i],listbuffer[i+1]);
		(*postproc_function) ();
		}
	}
    if (hop >= MAX_HOPS) quit(10,"Corrupted sector list\n\n");

    fclose(to_file);
    }

void a2in (void) {
    unsigned char listbuffer[256], databuffer[256];
    unsigned int i, curlist_trk, curlist_sec, listentry_pos, list_no;
    unsigned int curdata_trk, curdata_sec, procmode;
    unsigned int newlist_trk, newlist_sec;
    int c;
    new_sectors=list_no=procmode=0;
    if (!rawmode) {
	if (filetype==FILETYPE_T) procmode=1;
	else if (filetype==FILETYPE_B) procmode=2;
	else quit(11,"This type is supported only in raw mode.\n");
	}

#ifdef DOS
    extfilemode="r";
    if (procmode !=1) {
	extfilemode="rb";
	if (from_file)
	    quit(12,"stdin not allowed for binary input.\n");
	}
#else
    extfilemode="r";
#endif

    if (!from_file && !(from_file=fopen(extfilename,extfilemode))) {
	perror(extfilename);
	quit(13,"");
	}

    if (dir_do(dir_find_name)) quit(14,"File exists.\n");
    if (!dir_do(dir_find_space)) quit(15,"No space in directory.\n");
    if (padded_name[0] < 'A')
	quit(16,"Bad first filename character, must be >= 'A'.\n");
    for (i=0;i<30;i++)
	if (padded_name[i]==',')
	    quit(17,"Filename must not contain a comma.\n");
    for (i=0;i<30;i++) dir_entry_data[i+3]=padded_name[i]|0x80;
    dir_entry_data[2]=filetype;

    new_sector(&curlist_trk,&curlist_sec);
    dir_entry_data[0]=curlist_trk;
    dir_entry_data[1]=curlist_sec;
    for (i=0;i<256;i++) listbuffer[i]=0;
    listentry_pos=0;

    for (;;) {
	if (!rawmode || filetype!=FILETYPE_T) {
	    new_sector(&curdata_trk,&curdata_sec);
	    listbuffer[0x0c+(listentry_pos<<1)]=curdata_trk;
	    listbuffer[0x0d+(listentry_pos<<1)]=curdata_sec;
	    seek_sect(curdata_trk,curdata_sec);
	    if (preproc(procmode)) break;
	    }
	else {
	    /* Check for all-zero sectors for sparse T file */
	    for (i=0;i<256;i++) databuffer[i]=0;
	    i=0;
	    while((c=fgetc(from_file))!=EOF && i<256) databuffer[i++]=c;
	    while(i && !databuffer[i-1]) i--;
	    if (!i) {
		listbuffer[0x0c+(listentry_pos<<1)]=0;
		listbuffer[0x0d+(listentry_pos<<1)]=0;
		}
	    else {
		new_sector(&curdata_trk,&curdata_sec);
		listbuffer[0x0c+(listentry_pos<<1)]=curdata_trk;
		listbuffer[0x0d+(listentry_pos<<1)]=curdata_sec;
		write_sect(curdata_trk,curdata_sec,databuffer);
		}
	    if (c == EOF) break;
	    ungetc(c,from_file);
	    }
	if (++listentry_pos >= 0x7a) {
	    new_sector(&newlist_trk,&newlist_sec);
	    listbuffer[1]=newlist_trk;
	    listbuffer[2]=newlist_sec;
	    write_sect(curlist_trk,curlist_sec,listbuffer);
	    curlist_trk=newlist_trk;
	    curlist_sec=newlist_sec;
	    for (i=0;i<256;i++) listbuffer[i]=0;
	    listentry_pos=0;
	    listbuffer[5]=(++list_no*0x7a) & 0xff;
	    listbuffer[6]=(list_no*0x7a) >> 8;
	    }
	}

    listbuffer[1]=listbuffer[2]=0;
    write_sect(curlist_trk,curlist_sec,listbuffer);
    write_sect(0x11, 0, vtocbuffer);
    dir_entry_data[33]=new_sectors & 0xff;
    dir_entry_data[34]=new_sectors >> 8;
    fseek(image_fp,dir_entry_pos,SEEK_SET);
    /* writing ff first ensures directory is always in a safe state */
    fputc(0xff,image_fp);
    for (i=1;i<35;i++) fputc(dir_entry_data[i],image_fp);
    fseek(image_fp,dir_entry_pos,SEEK_SET);
    fputc(dir_entry_data[0],image_fp);

    fclose(from_file);
    }

int main (int argc, char *argv[]) {
    char *image_name, *image_mode, *a2_name, *basename, *typestr;
    unsigned int i, bad_vtoc;
    char *ls_cmd, *in_cmd, *out_cmd, *rm_cmd;
    char *ls_hlp, *in_hlp, *out_hlp, *rm_hlp, *general_hlp;
    int dos, x, image_rw=0;
    void (*command)(void) = NULL;

#ifdef DOS
    dos=1;
    ls_cmd="dir";
    in_cmd="in";
    out_cmd="out";
    rm_cmd="del";
    ls_hlp=in_hlp=out_hlp=rm_hlp=general_hlp=(char *) DOS_HelpText;
#else
    dos=0;
    general_hlp="Invoke as a2ls, a2in, a2out, or a2rm.\n";
    ls_cmd="a2ls";
    ls_hlp="Usage: a2ls <disk_image>\n";
    in_cmd="a2in";
    in_hlp=
    "Usage: a2in [-r] <type>[.<hex_addr>] <disk_image> <a2file> [<source>]\n";
    out_cmd="a2out";
    out_hlp="Usage: a2out [-r] <disk_image> <a2file> [<destination>]\n";
    rm_cmd="a2rm";
    rm_hlp="Usage: a2rm <disk_image> <a2file>\n";
#endif

    baseaddress=0x2000; /* default, hi-res page 1 */
    rawmode = begun = 0;
    extfilename = a2_name = image_name = "";

#ifdef DOS
    basename="";
    if (argc >=2) basename=argv[1];
#else
    basename=argv[0];
    /* strip off any leading directories */
    basename+=(i=strlen(basename));
    while(i-->0 && *--basename!='/');
    if (*basename=='/') basename++;
#endif

    if (!strcmp(basename,ls_cmd)) {
	if (argc !=2+dos) quit(18,ls_hlp);
	else {
	    image_name=argv[1+dos];
	    image_rw=0;
	    command= a2ls;
	    }
	}
    else if (!strcmp(basename,out_cmd)) {
	if (argc > 1+dos && !strcmp(argv[1+dos],"-r")) rawmode=1;
	x=3+dos+rawmode;
	if (argc != x && argc != x+1) quit(19,out_hlp);
	else {
	    image_name=argv[x-2];
	    image_rw=0;
	    a2_name=argv[x-1];
	    if (argc-x)
		extfilename=argv[x];
	    else
		to_file=stdout;
	    command= a2out;
	    }
	}
    else if (!strcmp(basename,in_cmd)) {
	if (argc > 1+dos && !strcmp(argv[1+dos],"-r")) rawmode=1;
	x=4+dos+rawmode;
	if (argc != x && argc != x+1) quit(20,in_hlp);
	else {
	    typestr=argv[x-3];
	    image_name=argv[x-2];
	    image_rw=1;
	    a2_name=argv[x-1];
	    if (argc-x)
		extfilename=argv[x];
	    else
		from_file=stdin;
	    switch(typestr[0]|0x20) {
		case 't': filetype=FILETYPE_T; break;
		case 'i': filetype=FILETYPE_I; break;
		case 'a': filetype=FILETYPE_A; break;
		case 'b': filetype=FILETYPE_B; break;
		case 's': filetype=FILETYPE_S; break;
		case 'r': filetype=FILETYPE_R; break;
		case 'x': filetype=FILETYPE_X; break;
		case 'y': filetype=FILETYPE_Y; break;
		default: quit(21,"<type>: one of t,i,a,b,s,r,x,y without -\n");
		}
	    if (typestr[1]=='.') {
		if (filetype==FILETYPE_B)
		    sscanf(&typestr[2],"%x",&baseaddress);
		else quit(22,"Base address applicable to type B only.\n");
		}
	    else if (typestr[1]!=0)
		quit(23,"The only modifier for <type> is .<hex_addr>\n");
	    command= a2in;
	    }
	}
    else if (!strcmp(basename,rm_cmd)) {
	if (argc != 3+dos) quit(24,rm_hlp);
	else {
	    image_name=argv[1+dos];
	    image_rw=1;
	    a2_name=argv[2+dos];
	    command= a2rm;
	    }
	}
    else
	quit(25,general_hlp);

    if (image_rw==1) image_mode="rb+"; else image_mode="rb";
    if (!(image_fp=fopen(image_name, image_mode)) || seek_sect(0, 0)) {
	perror(image_name);
	quit(26,"");
	}

    /* prepare source filename by padding blanks */
    i=0;
    while(i<30 && a2_name[i]) { padded_name[i]=a2_name[i] & 0x7f; i++; } // Compiler cleanup
    while(i<30) padded_name[i++]=' ';

    /* get VTOC and check validity */
    read_sect(0x11, 0, vtocbuffer);
    bad_vtoc=0;
    for (i=0; i<VTOC_CHK_NO; i++)
	bad_vtoc |= (vtocbuffer[vtoc_chk_offset[i]]!=vtoc_chk_value[i]);
    if (bad_vtoc)
	quit(27,"Not an Apple DOS 3.3 .dsk image.\n");

    (*command)();
    fclose(image_fp);
    return 0;
    }
