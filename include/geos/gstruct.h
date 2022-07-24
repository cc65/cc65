/*
  GEOS structs

  by Maciej 'YTM/Elysium' Witkowiak
*/

/*
   apart from initializing data, structures below can be used to
   speed up access to data and let cc65 to generate better code
   e.g. if you have menu defined as TopMenu and you want to change the number of
   menu items use:
    ((struct menu*)&TopMenu)->number=newNumber;
   This will be translated into single lda/sta pair
*/

#ifndef _GSTRUCT_H
#define _GSTRUCT_H

typedef void (*void_func) (void);

struct s_date {                 /* system date & time */
        char s_year;
        char s_month;
        char s_day;
        char s_hour;
        char s_minutes;
        char s_seconds;
};

struct tr_se {                  /* track and sector */
        char track;
        char sector;
};

struct fileheader {             /* header block (like fileHeader) */
        struct tr_se n_block;
        char icon_desc[3];
        char icon_pic[63];
        char dostype;
        char type;
        char structure;
        unsigned load_address;
        unsigned end_address;
        unsigned exec_address;
        char class_name[19];
        char column_flag;
        char author[63];
        char note[96];
};

#ifdef __GEOS_CBM__

struct f_date {                 /* date in filedesctiptor */
        char f_year;
        char f_month;
        char f_day;
        char f_hour;
        char f_minute;
};

struct filehandle {             /* filehandle in directory sectors */
        char dostype;           /* or in dirEntryBuf               */
        struct tr_se n_block;
        char name[16];
        struct tr_se header;
        char structure;
        char type;
        struct f_date date;
        unsigned size;
};

#else /* #ifdef __GEOS_CBM__ */

struct f_date {                 /* date in filedesctiptor */
        unsigned f_day:5;
        unsigned f_month:4;
        unsigned f_year:7;
        char f_minute;
        char f_hour;
};

struct filehandle {             /* filehandle in directory sectors */
        unsigned name_len:4;    /* or in dirEntryBuf               */
        unsigned structure:4;
        char name[15];
        char type;
        struct tr_se n_block;
        unsigned size;
        char byte_size[3];
        struct f_date date;
        char version;
        char min_version;
        char access;
        struct tr_se header;
        struct f_date mod_date;
        struct tr_se dir_head;
};

#endif /* #ifdef __GEOS_CBM__ */

struct pixel {                  /* describes point              */
        unsigned x;
        char y;
};

struct fontdesc {               /* describes font               */
        char baseline;
        char width;
        char height;
        char *index_tbl;
        char *data_ptr;
};

struct window {                 /* describes screen region      */
        char top;
        char bot;
        unsigned left;
        unsigned right;
};

struct VLIR_info {              /* VLIR information             */
        char curRecord;         /* currently only used in VLIR  */
        char usedRecords;       /* as system info (curRecord is mainly of your interest */
        char fileWritten;
        unsigned fileSize;
};

struct process {                /* process info, declare table of that type */
        unsigned pointer;       /* (like: struct process proctab[2]= ...    */
        unsigned jiffies;       /* last entry MUST BE {0,0}                 */
};

struct iconpic {                /* icon/encoded bitmap description          */
        char *pic_ptr;          /* ptr to a photo scrap (or encoded bitmap) */
        char x;                 /* position in cards (*8 pixels)            */
        char y;
        char width;             /* in cards                                 */
        char height;            /* in lines (pixels)                        */
};

struct icondef {                /* icon definition for DoIcons              */
        char *pic_ptr;          /* ptr to a photo scrap (or encoded bitmap) */
        char x;                 /* position in cards (*8 pixels)            */
        char y;
        char width;             /* of icon (in cards)                       */
        char height;            /* of icon in lines (pixels)                */
        unsigned proc_ptr;      /* pointer to function handling that icon   */
};

struct icontab {
        char number;            /* number of declared icons                 */
        struct pixel mousepos;  /* position of mouse after DoIcons          */
        struct icondef tab[];   /* table of size declared by icontab.number */
};

struct menuitem {
        char *name;
        char type;
        void *rest;             /* may be ptr to function, or ptr to struct menu (submenu) */
};

struct menu {
        struct window size;
        char number;
        struct menuitem items[];
};

struct inittab {                /* use struct inittab mytab[n] for initram              */
        unsigned ptr;           /* ptr to 1st byte                                      */
        char number;            /* number of following bytes                            */
        char values[];          /* actual string of bytes                               */
};

#endif
