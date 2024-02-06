/*
  GEOS dialog box functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GDLGBOX_H
#define _GDLGBOX_H

char __fastcall__ DoDlgBox(const char *dboxstring);
char RstrFrmDialogue(void);

/* These are custom, predefined dialog boxes, I'm sure you'll find them usable
   Most of them show 2 lines of text */

char __fastcall__ DlgBoxYesNo(const char *line1, const char *line2);
char __fastcall__ DlgBoxOkCancel(const char *line1, const char *line2);
void __fastcall__ DlgBoxOk(const char *line1, const char *line2);
char __fastcall__ DlgBoxGetString(char *myString, char strLength,
                                  const char *line1, const char *line2);
char __fastcall__ DlgBoxFileSelect(const char *classtxt, char ftype,
                                   char *fname);

/* This is a more general dialog box, works like printf in a window */
char MessageBox(char mode, const char *format, ...);

/* mode argument for MessageBox() */
enum {
    MB_EMPTY=0,
    MB_OK,
    MB_OKCANCEL,
    MB_YESNO,
    MB_LAST };

/* Now the command string type */

typedef void dlgBoxStr;

/* and command string commands - macros */

#define DB_DEFPOS(pattern) (char)(DEF_DB_POS | (pattern))
#define DB_SETPOS(pattern,top,bot,left,right) \
        (char)(SET_DB_POS | (pattern)), (char)(top), (char)(bot), \
        (unsigned)(left), (unsigned)(right)
#define DB_ICON(i,x,y)  (char)(i), (char)(x), (char)(y)
#define DB_TXTSTR(x,y,text) (char)DBTXTSTR, (char)(x), (char)(y), (text)
#define DB_VARSTR(x,y,ptr) (char)DBVARSTR, (char)(x), (char)(y), (char)(ptr)
#define DB_GETSTR(x,y,ptr,length) (char)DBGETSTRING, (char)(x), (char)(y), (char)(ptr), (char)(length)
#define DB_SYSOPV(ptr) (char)DBSYSOPV, (unsigned)(ptr)
#define DB_GRPHSTR(ptr) (char)DBGRPHSTR, (unsigned)(ptr)
#define DB_GETFILES(x,y) (char)DBGETFILES, (char)(x), (char)(y)
#define DB_OPVEC(ptr) (char)DBOPVEC, (unsigned)(ptr)
#define DB_USRICON(x,y,ptr) (char)DBUSRICON, (char)(x), (char)(y), (unsigned)(ptr)
#define DB_USRROUT(ptr) (char)DB_USR_ROUT, (unsigned)(ptr)
#define DB_END (char)NULL

/*
  part of constants below is used internally, but some are useful for macros above
*/

/* icons for DB_ICON */
#define OK              1
#define CANCEL          2
#define YES             3
#define NO              4
#define OPEN            5
#define DISK            6
/* commands - internally used by command macros */
#define DBTXTSTR        11
#define DBVARSTR        12
#define DBGETSTRING     13
#define DBSYSOPV        14
#define DBGRPHSTR       15
#define DBGETFILES      16
#define DBOPVEC         17
#define DBUSRICON       18
#define DB_USR_ROUT     19
/* icons tabulation in standard window */
#define DBI_X_0         1
#define DBI_X_1         9
#define DBI_X_2         17
#define DBI_Y_0         8
#define DBI_Y_1         40
#define DBI_Y_2         72
/* standard window size defaults */
#define SET_DB_POS      0
#define DEF_DB_POS      0x80
#define DEF_DB_TOP      32
#define DEF_DB_BOT      127
#define DEF_DB_LEFT     64
#define DEF_DB_RIGHT    255
/* text tabulation in standard window */
#define TXT_LN_1_Y      16
#define TXT_LN_2_Y      32
#define TXT_LN_3_Y      48
#define TXT_LN_4_Y      64
#define TXT_LN_5_Y      80
#define TXT_LN_X        16
/* system icons size */
#define SYSDBI_HEIGHT   16
#define SYSDBI_WIDTH    6
/* dialogbox string offsets */
#define OFF_DB_FORM     0
#define OFF_DB_TOP      1
#define OFF_DB_BOT      2
#define OFF_DB_LEFT     3
#define OFF_DB_RIGHT    5
#define OFF_DB_1STCMD   7

#endif
