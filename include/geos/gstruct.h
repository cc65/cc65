/*
  GEOS structs

  ported to small C on 25-27.10.1999
  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GSTRUCT_H
#define _GSTRUCT_H

struct f_date {			/* date in filedesctiptor */
	char f_year;
	char f_month;
	char f_day;
	char f_hour;
	char f_minute;
};

struct s_date {			/* system date & time */
	char s_year;
	char s_month;
	char s_day;
	char s_hour;
	char s_minutes;
	char s_seconds;
};

struct tr_se {			/* track and sector */
	char track;
	char sector;
};

struct fileheader {		/* header block (like fileHeader) */
	struct tr_se n_block;
	char icon_desc[3];
	char icon_pic[63];
	char dostype;
	char type;
	char structure;
	int load_address;
	int end_address;
	int exec_address;
	char class_name[19];
	char column_flag;
	char author[64];
	char note[95];
};

struct filehandle {		/* filehandle in directory sectors */
	char dostype;		/* or in dirEntryBuf               */
	struct tr_se n_block;
	char name[16];
	struct tr_se header;
	char structure;
	char type;
	struct f_date date;
	int size;
};

struct pixel {			/* describes point              */
	int x;
	char y;
};

struct fontdesc {		/* describes font               */
	char baseline;
	char width;
	char height;
	char *index_tbl;
	char *data_ptr;
};

struct window {			/* describes screen region      */
	char top;
	char bot;
	int left;
	int right;
};

struct VLIR_info {		/* VLIR information             */
	char curRecord;		/* currently only used in VLIR  */
	char usedRecords;	/* as system info (curRecord is mainly of your interest */
	char fileWritten;
	int fileSize;
};

struct process {		/* process info, declare table of that type */
	int pointer;		/* (like: struct process proctab[2]=...    */
	int jiffies;		/* last entry HAVE TO BE {0,0}              */
};


struct iconpic {		/* icon/encoded bitmap description          */
	char *pic_ptr;		/* ptr to a photo scrap (or encoded bitmap) */
	char x;			/* position in cards (*8 pixels)            */
	char y;
	char width;		/* in cards                                 */
	char heigth;		/* in lines (pixels)                        */
};

struct icondef {		/* icon definition for DoIcons              */
	char *pic_ptr;		/* ptr to a photo scrap (or encoded bitmap) */
	char x;			/* position in cards (*8 pixels)            */
	char y;
	char width;		/* of icon (in cards)                       */
	char heigth;		/* of icon in lines (pixels)                */
	int proc_ptr;		/* pointer to function handling that icon   */
};

struct icontab {
	char number;		/* number of declared icons                 */
	struct pixel mousepos;	/* position of mouse after DoIcons          */
	struct icondef tab[];	/* table of size declared by icontab.number */
};

/*
   structures below might be used to speed up access to own menus
   e.g. if you have menu defined as TopMenu and you want to change the number of
   menu items use:
    ((struct menu*)&TopMenu)->number=newNumber;
   This will allow cc65 to emit better code.
*/

struct menuitem {
	char *name;
	char type;
	int rest;		/* may be ptr to function, or if submenu ptr to struct menu */
};

struct menu {
	struct window size;
	char number;
	struct menuitem items[];
};

struct inittab {		/* use struct inittab mytab[n] for initram              */
	int ptr;		/* ptr to 1st byte                                      */
	char number;		/* number of following bytes                            */
	char values[];		/* warning - in table size of this is same for all!     */
};

#endif
