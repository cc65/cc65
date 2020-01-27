/*****************************************************************************/
/*                                                                           */
/*                  _atarios.h                                               */
/*                                                                           */
/*            Internal include file, do not use directly                     */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/

#ifndef __ATARIOS_H
#define __ATARIOS_H


/* IOCB Command Codes */

#define IOCB_OPEN        0x03  /* open */
#define IOCB_GETREC      0x05  /* get record */
#define IOCB_GETCHR      0x07  /* get character(s) */
#define IOCB_PUTREC      0x09  /* put record */
#define IOCB_PUTCHR      0x0B  /* put character(s) */
#define IOCB_CLOSE       0x0C  /* close */
#define IOCB_STATIS      0x0D  /* status */
#define IOCB_SPECIL      0x0E  /* special */
#define IOCB_DRAWLN      0x11  /* draw line */
#define IOCB_FILLIN      0x12  /* draw line with right fill */
#define IOCB_RENAME      0x20  /* rename disk file */
#define IOCB_DELETE      0x21  /* delete disk file */
#define IOCB_LOCKFL      0x23  /* lock file (set to read-only) */
#define IOCB_UNLOCK      0x24  /* unlock file */
#define IOCB_POINT       0x25  /* point sector */
#define IOCB_NOTE        0x26  /* note sector */
#define IOCB_GETFL       0x27  /* get file length */
#define IOCB_CHDIR_MYDOS 0x29  /* change directory (MyDOS) */
#define IOCB_MKDIR       0x2A  /* make directory (MyDOS/SpartaDOS) */
#define IOCB_RMDIR       0x2B  /* remove directory (SpartaDOS) */
#define IOCB_CHDIR_SPDOS 0x2C  /* change directory (SpartaDOS) */
#define IOCB_GETCWD      0x30  /* get current directory (MyDOS/SpartaDOS) */
#define IOCB_FORMAT      0xFE  /* format */


/* Device control block */

struct __dcb {
    unsigned char ddevic;           /* device id */
    unsigned char dunit;            /* unit number */
    unsigned char dcomnd;           /* command */
    unsigned char dstats;           /* command type / status return */
    void          *dbuf;            /* pointer to buffer */
    unsigned char dtimlo;           /* device timeout in seconds */
    unsigned char dunuse;           /* - unused - */
    unsigned int  dbyt;             /* # of bytes to transfer */
    union {    
        struct {
            unsigned char daux1;    /* 1st command auxiliary byte */
            unsigned char daux2;    /* 2nd command auxiliary byte */
        };
        unsigned int daux;          /* auxiliary as word */
    };
};

typedef struct __dcb dcb_t;


/* I/O control block */

struct __iocb {
    unsigned char   handler;    /* handler index number (0xff free) */
    unsigned char   drive;      /* device number (drive) */
    unsigned char   command;    /* command */
    unsigned char   status;     /* status of last operation */
    void*           buffer;     /* pointer to buffer */
    void*           put_byte;   /* pointer to device's PUT BYTE routine */
    unsigned int    buflen;     /* length of buffer */
    unsigned char   aux1;       /* 1st auxiliary byte */
    unsigned char   aux2;       /* 2nd auxiliary byte */
    unsigned char   aux3;       /* 3rd auxiliary byte */
    unsigned char   aux4;       /* 4th auxiliary byte */
    unsigned char   aux5;       /* 5th auxiliary byte */
    unsigned char   spare;      /* spare byte */
};

typedef struct __iocb iocb_t;


/* DOS 2.x zeropage variables */

struct __dos2x {
    unsigned char*  zbufp;      /* points to user filename */
    unsigned char*  zdrva;      /* points to serveral buffers (mostly VTOC) */
    unsigned char*  zsba;       /* points to sector buffer */
    unsigned char   errno;      /* number of occured error */
};

typedef struct __dos2x dos2x_t;


/* A single device handler formed by it's routines */

struct __devhdl {
    void *open;                 /* address of OPEN routine -1 */
    void *close;                /* address of CLOSE routine -1 */
    void *get;                  /* address of GET BYTE routine -1 */
    void *put;                  /* address of PUT BYTE routine -1 */
    void *status;               /* address of GET STATUS routine -1 */
    void *special;              /* address od SPECIAL routine -1 */
    unsigned char jmp_inst;     /* a "JMP" byte, should be $4C */
    void (*init)(void);         /* init routine (JMP INIT) */
    unsigned char reserved;     /* unused */
};

typedef struct __devhdl devhdl_t;


/* List of device handlers, as managed in HATABS */

struct __hatabs {
    unsigned char   id;         /* ATASCII code of handler e.g. 'C','D','E','K','P','S','R' */
    devhdl_t*       devhdl;     /* Pointer to routines of device */
};

typedef struct __hatabs hatabs_t;


/* Floating point register */

struct __fpreg {
#ifdef OS_REV2
    unsigned char fr;
    unsigned char frm[5];       /* 5-byte register mantissa */
#else
    unsigned char fr[6];        /* 6 bytes for single register */
#endif
};

typedef struct __fpreg fpreg_t;

enum {                          /* enum for access of floating point registers */
    FPIDX_R0 = 0,               /* (to use as index) */
    FPIDX_RE = 1,
    FPIDX_R1 = 2,
    FPIDX_R2 = 3
};


/* Define a structure with atari os register offsets */

struct __os {

    // --- Zero-Page ---

#ifdef OSA
    unsigned char*  linzbs;                 // = $00/$01        LINBUG RAM (WILL BE REPLACED BY MONITOR RAM)
#else           
    unsigned char   linflg;                 // = $00            LNBUG FLAG (0 = NOT LNBUG)
    unsigned char   ngflag;                 // = $01            MEMORY STATUS (0 = FAILURE)
#endif          
    unsigned char*  casini;                 // = $02/$03        CASSETTE INIT LOCATION
    unsigned char*  ramlo;                  // = $04/$05        RAM POINTER FOR MEMORY TEST
                
#ifdef OSA           
    unsigned char   tramsz;                 // = $06            FLAG FOR LEFT CARTRIDGE
    unsigned char   tstdat;                 // = $07            FLAG FOR RIGHT CARTRIDGE
#else           
    unsigned char   trnsmz;                 // = $06            TEMPORARY REGISTER FOR RAM SIZE
    unsigned char   tstdat;                 // = $07            UNUSED (NOT TOUCHED DURING RESET/COLD START)
#endif
            
    // Cleared upon Coldstart only                
                    
    unsigned char   warmst;                 // = $08            WARM START FLAG
    unsigned char   bootq;                  // = $09            SUCCESSFUL BOOT FLAG   
    void (*dosvec)(void);                   // = $0A/$0B        DISK SOFTWARE START VECTOR
    void (*dosini)(void);                   // = $0C/$0D        DISK SOFTWARE INIT ADDRESS
    unsigned char*  appmhi;                 // = $0E/$0F        APPLICATIONS MEMORY HI LIMIT    

    // Cleared upon Coldstart or Warmstart

    unsigned char   pokmsk;                 // = $10            SYSTEM MASK FOR POKEY IRQ ENABLE
    unsigned char   brkkey;                 // = $11            BREAK KEY FLAG
    unsigned char   rtclok[3];              // = $12-$14        REAL TIME CLOCK (IN 16 MSEC UNITS)
    unsigned char*  bufadr;                 // = $15/$16        INDIRECT BUFFER ADDRESS REGISTER
    unsigned char   iccomt;                 // = $17            COMMAND FOR VECTOR
    unsigned char*  dskfms;                 // = $18/$19        DISK FILE MANAGER POINTER
    unsigned char*  dskutl;                 // = $1A/$1B        DISK UTILITIES POINTER
#ifdef OSA   
    unsigned char   ptimot;                 // = $1C            PRINTER TIME OUT REGISTER
    unsigned char   pbpnt;                  // = $1D            PRINT BUFFER POINTER
    unsigned char   pbufsz;                 // = $1E            PRINT BUFFER SIZE
    unsigned char   ptemp;                  // = $1F            TEMPORARY REGISTER
#else               
    unsigned char   abufpt[4];              // = $1C-$1F        ACMI BUFFER POINTER AREA
#endif              
    iocb_t          ziocb;                  // = $20-$2F        ZERO PAGE I/O CONTROL BLOCK
                    
    unsigned char   status;                 // = $30            INTERNAL STATUS STORAGE
    unsigned char   chksum;                 // = $31            CHECKSUM (SINGLE BYTE SUM WITH CARRY)
    unsigned char*  bufr;                   // = $32/$33        POINTER TO DATA BUFFER
    unsigned char*  bfen;                   // = $34/$35        NEXT BYTE PAST END OF THE DATA BUFFER LO
#ifdef OSA                   
    unsigned char   cretry;                 // = $36            NUMBER OF COMMAND FRAME RETRIES
    unsigned char   dretry;                 // = $37            NUMBER OF DEVICE RETRIES
#else               
    unsigned int    ltemp;                  // = $36/$37        LOADER TEMPORARY
#endif              
    unsigned char   bufrfl;                 // = $38            DATA BUFFER FULL FLAG
    unsigned char   recvdn;                 // = $39            RECEIVE DONE FLAG
    unsigned char   xmtdon;                 // = $3A            TRANSMISSION DONE FLAG
    unsigned char   chksnt;                 // = $3B            CHECKSUM SENT FLAG
    unsigned char   nocksm;                 // = $3C            NO CHECKSUM FOLLOWS DATA FLAG
    unsigned char   bptr;                   // = $3D            CASSETTE BUFFER POINTER
    unsigned char   ftype;                  // = $3E            CASSETTE IRG TYPE
    unsigned char   feof;                   // = $3F            CASSETTE EOF FLAG (0 // = QUIET)
                    
    unsigned char   freq;                   // = $40            CASSETTE BEEP COUNTER
    unsigned char   soundr;                 // = $41            NOISY I/0 FLAG. (ZERO IS QUIET)
    unsigned char   critic;                 // = $42            DEFINES CRITICAL SECTION (CRITICAL IF NON-Z)
    dos2x_t         fmszpg;                 // = $43-$49        DISK FILE MANAGER SYSTEM ZERO PAGE
#ifdef OSA                   
    unsigned char   ckey;                   // = $4A            FLAG SET WHEN GAME START PRESSED
    unsigned char   cassbt;                 // = $4B            CASSETTE BOOT FLAG
#else               
    void*           zchain;                 // = $4A/$4B        HANDLER LINKAGE CHAIN POINTER
#endif              
    unsigned char   dstat;                  // = $4C            DISPLAY STATUS
    unsigned char   atract;                 // = $4D            ATRACT FLAG
    unsigned char   drkmsk;                 // = $4E            DARK ATRACT MASK
    unsigned char   colrsh;                 // = $4F            ATRACT COLOR SHIFTER (EOR'ED WITH PLAYFIELD
                
    unsigned char   tmpchr;                 // = $50            TEMPORARY CHARACTER
    unsigned char   hold1;                  // = $51            TEMPORARY
    unsigned char   lmargn;                 // = $52            LEFT MARGIN (NORMALLY 2, CC65 C STARTUP CODE SETS IT TO 0)
    unsigned char   rmargn;                 // = $53            RIGHT MARGIN (NORMALLY 39 IF NO XEP80 IS USED)
    unsigned char   rowcrs;                 // = $54            1CURSOR ROW
    unsigned int    colcrs;                 // = $55/$56        CURSOR COLUMN
    unsigned char   dindex;                 // = $57            DISPLAY MODE
    unsigned char*  savmsc;                 // = $58/$59        SAVED MEMORY SCAN COUNTER
    unsigned char   oldrow;                 // = $5A            PRIOR ROW
    unsigned int    oldcol;                 // = $5B/$5C        PRIOR COLUMN
    unsigned char   oldchr;                 // = $5D            DATA UNDER CURSOR
    unsigned char*  oldadr;                 // = $5E/$5F        SAVED CURSOR MEMORY ADDRESS
    
#ifdef OSA 
    unsigned char   newrow;                 // = $60            POINT DRAW GOES TO
    unsigned int    newcol;                 // = $61/$62        COLUMN DRAW GOES TO
#else               
    unsigned char*  fkdef;                  // = $60/$61        FUNCTION KEY DEFINITION TABLE
    unsigned char   palnts;                 // = $62            PAL/NTSC INDICATOR (0 // = NTSC)
#endif          
    unsigned char   logcol;                 // = $63            POINTS AT COLUMN IN LOGICAL LINE
    unsigned char*  adress;                 // = $64/$65        TEMPORARY ADDRESS
    unsigned int    mlttmp;                 // = $66/$67        TEMPORARY / FIRST BYTE IS USED IN OPEN AS TEMP          
    unsigned int    savadr;                 // = $68/$69        SAVED ADDRESS        
    unsigned char   ramtop;                 // = $6A            RAM SIZE DEFINED BY POWER ON LOGIC
    unsigned char   bufcnt;                 // = $6B            BUFFER COUNT
    unsigned char*  bufstr;                 // = $6C/$6D        EDITOR GETCH POINTER
    unsigned char   bitmsk;                 // = $6E            BIT MASK
    unsigned char   shfamt;                 // = $6F            SHIFT AMOUNT FOR PIXEL JUSTIFUCATION
                
    unsigned int    rowac;                  // = $70/$71        DRAW WORKING ROW
    unsigned int    colac;                  // = $72/$73        DRAW WORKING COLUMN
    unsigned char*  endpt;                  // = $74/$75        END POINT
    unsigned char   deltar;                 // = $76            ROW DIFFERENCE
    unsigned int    deltac;                 // = $77/$78        COLUMN DIFFERENCE
#ifdef OSA               
    unsigned char   rowinc;                 // = $79            ROWINC 
    unsigned char   colinc;                 // = $7A            COLINC
#else           
    unsigned char*  keydef;                 // = $79/$7A        2-BYTE KEY DEFINITION TABLE ADDRESS
#endif          
    unsigned char   swpflg;                 // = $7B            NON-0 1F TXT AND REGULAR RAM IS SWAPPED
    unsigned char   holdch;                 // = $7C            CH IS MOVED HERE IN KGETCH BEFORE CNTL & SH
    unsigned char   insdat;                 // = $7D            1-BYTE TEMPORARY
    unsigned int    countr;                 // = $7E/$7F        2-BYTE DRAW ITERATION COUNT
    
    unsigned char   _free_1[0xD4-0x7F-1];   // USER SPACE
    
                                            // Floating Point Package Page Zero Address Equates
    fpreg_t         fpreg[4];               // = $D4-$EB        4 REGSITERS, ACCCESS LIKE "fpreg[FPIDX_R0].fr"        
    unsigned char   frx;                    // = $EC            1-BYTE TEMPORARY   
    unsigned char   eexp;                   // = $ED            VALUE OF EXP
#ifdef OS_REV2          
    unsigned char   frsign;                 // = $EE            ##REV2## 1-BYTE FLOATING POINT SIGN
    unsigned char   plycnt;                 // = $EF            ##REV2## 1-BYTE POLYNOMIAL DEGREE
    unsigned char   sgnflg;                 // = $F0            ##REV2## 1-BYTE SIGN FLAG
    unsigned char   xfmflg;                 // = $F1            ##REV2## 1-BYTE TRANSFORM FLAG
#else           
    unsigned char   nsign;                  // = $EE            SIGN OF #
    unsigned char   esign;                  // = $EF            SIGN OF EXPONENT
    unsigned char   fchrflg;                // = $F0            1ST CHAR FLAG
    unsigned char   digrt;                  // = $F1            # OF DIGITS RIGHT OF DECIMAL
#endif          
    unsigned char   cix;                    // = $F2            CURRENT INPUT INDEX
    unsigned char*  inbuff;                 // = $F3/$F4        POINTS TO USER'S LINE INPUT BUFFER                           
    unsigned int    ztemp1;                 // = $F5/$F6        2-BYTE TEMPORARY
    unsigned int    ztemp4;                 // = $F7/$F8        2-BYTE TEMPORARY
    unsigned int    ztemp3;                 // = $F9/$FA        2-BYTE TEMPORARY
                
    union {         
        unsigned char   degflg;             // = $FB            ##OLD## SAME AS RADFLG
        unsigned char   radflg;             // = $FB            ##OLD## 0=RADIANS, 6=DEGREES
    };
            
    fpreg_t*        flptr;                  // = $FC/$FD        2-BYTE FLOATING POINT NUMBER POINTER
    fpreg_t*        fptr2;                  // = $FE/$FF        2-BYTE FLOATING POINT NUMBER POINTER

    // --- Page 1 ---

    unsigned char   stack[0x100];           // STACK

    // --- Page 2 ---

    void (*vdslst)(void);                   // = $0200/$0201    DISPLAY LIST NMI VECTOR
    void (*vprced)(void);                   // = $0202/$0203    PROCEED LINE IRQ VECTOR
    void (*vinter)(void);                   // = $0204/$0205    INTERRUPT LINE IRQ VECTOR
    void (*vbreak)(void);                   // = $0206/$0207    SOFTWARE BREAK (00) INSTRUCTION IRQ VECTOR
    void (*vkeybd)(void);                   // = $0208/$0209    POKEY KEYBOARD IRQ VECTOR
    void (*vserin)(void);                   // = $020A/$020B    POKEY SERIAL INPUT READY IRQ
    void (*vseror)(void);                   // = $020C/$020D    POKEY SERIAL OUTPUT READY IRQ
    void (*vseroc)(void);                   // = $020E/$020F    POKEY SERIAL OUTPUT COMPLETE IRQ
    void (*vtimr1)(void);                   // = $0210/$0201    POKEY TIMER 1 IRQ
    void (*vtimr2)(void);                   // = $0212/$0203    POKEY TIMER 2 IRQ
    void (*vtimr4)(void);                   // = $0214/$0205    POKEY TIMER 4 IRQ
    void (*vimirq)(void);                   // = $0216/$0207    IMMEDIATE IRQ VECTOR
    unsigned int cdtmv1;                    // = $0218/$0210    COUNT DOWN TIMER 1
    unsigned int cdtmv2;                    // = $021A/$021B    COUNT DOWN TIMER 2
    unsigned int cdtmv3;                    // = $021C/$021D    COUNT DOWN TIMER 3
    unsigned int cdtmv4;                    // = $021E/$021F    COUNT DOWN TIMER 4
    unsigned int cdtmv5;                    // = $0220/$0221    COUNT DOWN TIMER 5
    void (*vvblki)(void);                   // = $0222/$0223    IMMEDIATE VERTICAL BLANK NMI VECTOR
    void (*vvblkd)(void);                   // = $0224/$0224    DEFERRED VERTICAL BLANK NMI VECTOR
    void (*cdtma1)(void);                   // = $0226/$0227    COUNT DOWN TIMER 1 JSR ADDRESS
    void (*cdtma2)(void);                   // = $0228/$0229    COUNT DOWN TIMER 2 JSR ADDRESS
    unsigned char cdtmf3;                   // = $022A          COUNT DOWN TIMER 3 FLAG
    unsigned char srtimr;                   // = $022B          SOFTWARE REPEAT TIMER
    unsigned char cdtmf4;                   // = $022C          COUNT DOWN TIMER 4 FLAG
    unsigned char intemp;                   // = $022D          IAN'S TEMP
    unsigned char cdtmf5;                   // = $022E          COUNT DOWN TIMER FLAG 5
    unsigned char sdmctl;                   // = $022F          SAVE DMACTL REGISTER
    union {
        struct {
            unsigned char sdlstl;           // = $0230          SAVE DISPLAY LIST LOW BYTE
            unsigned char sdlsth;           // = $0231          SAVE DISPLAY LIST HI BYTE     
        };
        void*   sdlst;                      // = $0230/$0231    (same as above as pointer)
    };
    unsigned char sskctl;                   // = $0232          SKCTL REGISTER RAM
#ifdef OSA 
    unsigned char _spare_1;                 // = $0233          No OS use.
#else
    unsigned char lcount;                   // = $0233          ##1200xl## 1-byte relocating loader record
#endif          
    unsigned char lpenh;                    // = $0234          LIGHT PEN HORIZONTAL VALUE
    unsigned char lpenv;                    // = $0235          LIGHT PEN VERTICAL VALUE
    void (*brkky)(void);                    // = $0236/$0237    BREAK KEY VECTOR
#ifdef OSA           
    unsigned char spare2[2];                // = $0238/$0239    No OS use.
#else           
    void (*vpirq)(void);                    // = $0238/$0239    ##rev2## 2-byte parallel device IRQ vector
#endif          
    unsigned char cdevic;                   // = $023A          COMMAND FRAME BUFFER - DEVICE
    unsigned char ccomnd;                   // = $023B          COMMAND
    union {
        struct {    
            unsigned char caux1;            // = $023C          COMMAND AUX BYTE 1
            unsigned char caux2;            // = $023D          COMMAND AUX BYTE 2
        };
        unsigned int caux;                  // = $023C/$023D    (same as above as word)
    };
    unsigned char temp;                     // = $023E          TEMPORARY RAM CELL
    unsigned char errflg;                   // = $023F          ERROR FLAG - ANY DEVICE ERROR EXCEPT TIME OUT
    unsigned char dflags;                   // = $0240          DISK FLAGS FROM SECTOR ONE
    unsigned char dbsect;                   // = $0241          NUMBER OF DISK BOOT SECTORS
    unsigned char* bootad;                  // = $0242/$0243    ADDRESS WHERE DISK BOOT LOADER WILL BE PUT
    unsigned char coldst;                   // = $0244          COLDSTART FLAG (1=IN MIDDLE OF COLDSTART>
#ifdef OSA           
    unsigned char spare3;                   // = $0245          No OS use.
#else           
    unsigned char reclen;                   // = $0245          ##1200xl## 1-byte relocating loader record length
#endif              
    unsigned char dsktim;                   // = $0246          DISK TIME OUT REGISTER
#ifdef OSA               
    unsigned char linbuf[40];               // = $0247-$026E    ##old## CHAR LINE BUFFER
#else           
    unsigned char pdvmsk;                   // = $0247          ##rev2## 1-byte parallel device selection mask
    unsigned char shpdvs;                   // = $0248          ##rev2## 1-byte PDVS (parallel device select)
    unsigned char pdimsk;                   // = $0249          ##rev2## 1-byte parallel device IRQ selection
    unsigned int  reladr;                   // = $024A/$024B    ##rev2## 2-byte relocating loader relative adr.
    unsigned char pptmpa;                   // = $024C          ##rev2## 1-byte parallel device handler temporary
    unsigned char pptmpx;                   // = $024D          ##rev2## 1-byte parallel device handler temporary
    unsigned char _reserved_1[29];          // = $024E-$026A    RESERVED
    unsigned char chsalt;                   // = $026B          ##1200xl## 1-byte character set alternate
    unsigned char vsflag;                   // = $026C          ##1200xl## 1-byte fine vertical scroll count
    unsigned char keydis;                   // = $026D          ##1200xl## 1-byte keyboard disable
    unsigned char fine;                     // = $026E          ##1200xl## 1-byte fine scrolling mode
#endif              
    unsigned char gprior;                   // = $026F          GLOBAL PRIORITY CELL
    unsigned char paddl0;                   // = $0270          1-BYTE POTENTIOMETER 0
    unsigned char paddl1;                   // = $0271          1-BYTE POTENTIOMETER 1
    unsigned char paddl2;                   // = $0272          1-BYTE POTENTIOMETER 2
    unsigned char paddl3;                   // = $0273          1-BYTE POTENTIOMETER 3
    unsigned char paddl4;                   // = $0274          1-BYTE POTENTIOMETER 4
    unsigned char paddl5;                   // = $0275          1-BYTE POTENTIOMETER 5
    unsigned char paddl6;                   // = $0276          1-BYTE POTENTIOMETER 6
    unsigned char paddl7;                   // = $0277          1-BYTE POTENTIOMETER 7
    unsigned char stick0;                   // = $0278          1-byte joystick 0
    unsigned char stick1;                   // = $0279          1-byte joystick 1
    unsigned char stick2;                   // = $027A          1-byte joystick 2
    unsigned char stick3;                   // = $027B          1-byte joystick 3
    unsigned char ptrig0;                   // = $027C          1-BYTE PADDLE TRIGGER 0
    unsigned char ptrig1;                   // = $027D          1-BYTE PADDLE TRIGGER 1
    unsigned char ptrig2;                   // = $027E          1-BYTE PADDLE TRIGGER 2
    unsigned char ptrig3;                   // = $027F          1-BYTE PADDLE TRIGGER 3
    unsigned char ptrig4;                   // = $0280          1-BYTE PADDLE TRIGGER 4
    unsigned char ptrig5;                   // = $0281          1-BYTE PADDLE TRIGGER 5
    unsigned char ptrig6;                   // = $0281          1-BYTE PADDLE TRIGGER 6
    unsigned char ptrig7;                   // = $0283          1-BYTE PADDLE TRIGGER 7
    unsigned char strig0;                   // = $0284          1-BYTE JOYSTICK TRIGGER 0
    unsigned char strig1;                   // = $0285          1-BYTE JOYSTICK TRIGGER 1
    unsigned char strig2;                   // = $0286          1-BYTE JOYSTICK TRIGGER 2
    unsigned char strig3;                   // = $0287          1-BYTE JOYSTICK TRIGGER 3
#ifdef OSA           
    unsigned char cstat;                    // = $0288          ##old## cassette status register
#else           
    unsigned char hibyte;                   // = $0288          ##1200xl## 1-byte relocating loader high byte
#endif          
    unsigned char wmode;                    // = $0289          1-byte cassette WRITE mode
    unsigned char blim;                     // = $028A          1-byte cassette buffer limit
#ifdef OSA
    unsigned char _reserved_2[5];           // = $028B-$028F    RESERVED
#else    
    unsigned char imask;                    // = $028B          ##rev2## (not used)
    void (*jveck)(void);                    // = $028C/$028D    2-byte jump vector
    unsigned newadr;                        // = $028E/028F     ##1200xl## 2-byte relocating address
#endif              
    unsigned char txtrow;                   // = $0290          TEXT ROWCRS
    unsigned txtcol;                        // = $0291/$0292    TEXT COLCRS
    unsigned char tindex;                   // = $0293          TEXT INDEX
    unsigned char* txtmsc;                  // = $0294/$0295    FOOLS CONVRT INTO NEW MSC
    unsigned char txtold[6];                // = $0296-$029B    OLDROW & OLDCOL FOR TEXT (AND THEN SOME)
#ifdef OSA               
    unsigned char tmpx1;                    // = $029C          ##old## 1--byte temporary register
#else           
    unsigned char cretry;                   // = $029C          ##1200xl## 1-byte number of command frame retries
#endif              
    unsigned char hold3;                    // = $029D          1-byte temporary
    unsigned char subtmp;                   // = $029E          1-byte temporary
    unsigned char hold2;                    // = $029F          1-byte (not used)
    unsigned char dmask;                    // = $02A0          1-byte display (pixel location) mask
    unsigned char tmplbt;                   // = $02A1          1-byte (not used)
    unsigned char escflg;                   // = $02A2          ESCAPE FLAG
    unsigned char tabmap[15];               // = $02A3-$02B1    15-byte (120 bit) tab stop bit map
    unsigned char logmap[4];                // = $02B2-$02B5    LOGICAL LINE START BIT MAP
    unsigned char invflg;                   // = $02B6          INVERSE VIDEO FLAG (TOGGLED BY ATARI KEY)
    unsigned char filflg;                   // = $02B7          RIGHT FILL FLAG FOR DRAW
    unsigned char tmprow;                   // = $02B8          1-byte temporary row
    unsigned tmpcol;                        // = $02B9/$02BA    2-byte temporary column
    unsigned char scrflg;                   // = $02BB          SET IF SCROLL OCCURS
    unsigned char hold4;                    // = $02BC          TEMP CELL USED IN DRAW ONLY
#ifdef OSA           
    unsigned char hold5;                    // = $02BD          ##old## DITTO
#else           
    unsigned char dretry;                   // = $02BD          ##1200xl## 1-byte number of device retries
#endif              
    unsigned char shflok;                   // = $02BE          1-byte shift/control lock flags
    unsigned char botscr;                   // = $02BF          BOTTOM OF SCREEN   24 NORM 4 SPLIT
    unsigned char pcolr0;                   // = $02C0          1-byte player-missile 0 color/luminance
    unsigned char pcolr1;                   // = $02C1          1-byte player-missile 1 color/luminance
    unsigned char pcolr2;                   // = $02C2          1-byte player-missile 2 color/luminance
    unsigned char pcolr3;                   // = $02C3          1-byte player-missile 3 color/luminance 
    unsigned char color0;                   // = $02C4          1-byte playfield 0 color/luminance
    unsigned char color1;                   // = $02C5          1-byte playfield 1 color/luminance
    unsigned char color2;                   // = $02C6          1-byte playfield 2 color/luminance
    unsigned char color3;                   // = $02C7          1-byte playfield 3 color/luminance
    unsigned char color4;                   // = $02C8          1-byte background color/luminance
#ifdef OSA 
    unsigned char _spare_2[23];             // = $02C9-$02DF    No OS use.
#else
    union {
        unsigned char parmbl[6];            // = $02C9          ##rev2## 6-byte relocating loader parameter
        struct {        
            void (*runadr)(void);           // = $02C9          ##1200xl## 2-byte run address
            unsigned int hiused;            // = $02CB          ##1200xl## 2-byte highest non-zero page address
            unsigned int zhiuse;            // = $02CD          ##1200xl## 2-byte highest zero page address
        };       
    };       
    union {     
        unsigned char oldpar[6];            // = $02CF          ##rev2## 6-byte relocating loader parameter
        struct {        
            void (*gbytea)(void);           // = $02CF          ##1200xl## 2-byte GET-BYTE routine address
            unsigned int loadad;            // = $02D1          ##1200xl## 2-byte non-zero page load address
            unsigned int zloada;            // = $02D3          ##1200xl## 2-byte zero page load address
        };       
    };       
    unsigned int dsctln;                    // = $02D5          ##1200xl## 2-byte disk sector length
    unsigned int acmisr;                    // = $02D7          ##1200xl## 2-byte ACMI interrupt service routine
    unsigned char krpdel;                   // = $02D9          ##1200xl## 1-byte auto-repeat delay
    unsigned char keyrep;                   // = $02DA          ##1200xl## 1-byte auto-repeat rate
    unsigned char noclik;                   // = $02DB          ##1200xl## 1-byte key click disable
    unsigned char helpfg;                   // = $02DC          ##1200xl## 1-byte HELP key flag (0 = no HELP)
    unsigned char dmasav;                   // = $02DD          ##1200xl## 1-byte SDMCTL save/restore
    unsigned char pbpnt;                    // = $02DE          ##1200xl## 1-byte printer buffer pointer
    unsigned char pbufsz;                   // = $02DF          ##1200xl## 1-byte printer buffer size
#endif      
    union {         
        unsigned char glbabs[4];            // = $02E0-$02E3    byte global variables for non-DOS users
        struct {        
            void (*runad)(void);            // = $02E0          ##map## 2-byte binary file run address
            void (*initad)(void);           // = $02E2          ##map## 2-byte binary file initialization address
        };      
    };    
    unsigned char ramsiz;                   // = $02E4          RAM SIZE (HI BYTE ONLY)
    void* memtop;                           // = $02E5          TOP OF AVAILABLE USER MEMORY
    void* memlo;                            // = $02E7          BOTTOM OF AVAILABLE USER MEMORY
#ifdef OSA       
    unsigned char _spare_3;                 // = $02E9          No OS use.
#else       
    unsigned char hndlod;                   // = $02E9          ##1200xl## 1-byte user load flag
#endif      
    unsigned char dvstat[4];                // = $02EA-$02ED    STATUS BUFFER
    union {     
        unsigned int cbaud;                 // = $02EE/$02EF    2-byte cassette baud rate
        struct {        
            unsigned char cbaudl;           // = $02EE          1-byte low cassette baud rate
            unsigned char cbaudh;           // = $02EF          1-byte high cassette baud rate
        };    
    };  
    unsigned char crsinh;                   // = $02F0          CURSOR INHIBIT (00 = CURSOR ON)
    unsigned char keydel;                   // = $02F1          KEY DELAY
    unsigned char ch1;                      // = $02F2          1-byte prior keyboard character
    unsigned char chact;                    // = $02F3          CHACTL REGISTER RAM
    unsigned char chbas;                    // = $02F4          CHBAS REGISTER RAM
#ifdef OSA       
    unsigned char _spare_4[5];              // = $02F5-$02F9    No OS use.
#else           
    unsigned char newrow;                   // = $02F5          ##1200xl## 1-byte draw destination row
    unsigned int  newcol;                   // = $02F6/$02F7    ##1200xl## 2-byte draw destination column
    unsigned char rowinc;                   // = $02F8          ##1200xl## 1-byte draw row increment
    unsigned char colinc;                   // = $02F9          ##1200xl## 1-byte draw column increment
#endif      
    unsigned char char_;                    // = $02FA          1-byte internal character (naming changed due to do keyword conflict)
    unsigned char atachr;                   // = $02FB          ATASCII CHARACTER
    unsigned char ch;                       // = $02FC          GLOBAL VARIABLE FOR KEYBOARD
    unsigned char fildat;                   // = $02FD          RIGHT FILL DATA <DRAW>
    unsigned char dspflg;                   // = $02FE          DISPLAY FLAG   DISPLAY CNTLS IF NON-ZERO
    unsigned char ssflag;                   // = $02FF          START/STOP FLAG FOR PAGING (CNTL 1). CLEARE
        
    // --- Page 3 ---

    dcb_t dcb;                              // = $0300-$030B    DEVICE CONTROL BLOCK
    unsigned int timer1;                    // = $030C/$030D    INITIAL TIMER VALUE
#ifdef OSA                      
    unsigned char addcor;                   // = $030E          ##old## ADDITION CORRECTION
#else                       
    unsigned char jmpers;                   // = $030E          ##1200xl## 1-byte jumper options
#endif          
    unsigned char casflg;                   // = $030F          CASSETTE MODE WHEN SET
    unsigned int  timer2;                   // = $0310/$0311    2-byte final baud rate timer value
    unsigned char temp1;                    // = $0312          TEMPORARY STORAGE REGISTER
#ifdef OSA                  
    unsigned char _spare_5;                 // = $0313          unused
    unsigned char temp2;                    // = $0314          ##old## TEMPORARY STORAGE REGISTER
#else                               
    unsigned char temp2;                    // = $0313          ##1200xl## 1-byte temporary
    unsigned char ptimot;                   // = $0314          ##1200xl## 1-byte printer timeout
#endif                  
    unsigned char temp3;                    // = $0315          TEMPORARY STORAGE REGISTER
    unsigned char savio;                    // = $0316          SAVE SERIAL IN DATA PORT
    unsigned char timflg;                   // = $0317          TIME OUT FLAG FOR BAUD RATE CORRECTION
    unsigned char stackp;                   // = $0318          SIO STACK POINTER SAVE CELL
    unsigned char tstat;                    // = $0319          TEMPORARY STATUS HOLDER
#ifdef OSA              
    hatabs_t      hatabs[12];               // = $031A-$033D    handler address table
    unsigned int  zeropad;                  // = $033E/$033F    zero padding
#else           
    hatabs_t      hatabs[11];               // = $031A-$033A    handler address table
    unsigned int  zeropad;                  // = $033B/$033C    zero padding
    unsigned char pupbt1;                   // = $033D          ##1200xl## 1-byte power-up validation byte 1
    unsigned char pupbt2;                   // = $033E          ##1200xl## 1-byte power-up validation byte 2
    unsigned char pupbt3;                   // = $033F          ##1200xl## 1-byte power-up validation byte 3
#endif

    iocb_t        iocb[8];                  // = $0340-$03BF    8 I/O Control Blocks
    unsigned char prnbuf[40];               // = $03C0-$3E7     PRINTER BUFFER
#ifdef OSA           
    unsigned char _spare_6[151];            // = $03E8-$047F    unused
#else               
    unsigned char superf;                   // = $03E8          ##1200xl## 1-byte editor super function flag
    unsigned char ckey;                     // = $03E9          ##1200xl## 1-byte cassette boot request flag
    unsigned char cassbt;                   // = $03EA          ##1200xl## 1-byte cassette boot flag
    unsigned char cartck;                   // = $03EB          ##1200xl## 1-byte cartridge equivalence check
    unsigned char derrf;                    // = $03EC          ##rev2## 1-byte screen OPEN error flag
    unsigned char acmvar[11];               // = $03ED-$03F7    ##1200xl## reserved for ACMI, not cleared upon reset
    unsigned char basicf;                   // = $03F8          ##rev2## 1-byte BASIC switch flag
    unsigned char mintlk;                   // = $03F9          ##1200xl## 1-byte ACMI module interlock
    unsigned char gintlk;                   // = $03FA          ##1200xl## 1-byte cartridge interlock
    void*         chlink;                   // = $03FB/$03FC    ##1200xl## 2-byte loaded handler chain link
    unsigned char casbuf[131];              // = $03FD-$047F    CASSETTE BUFFER
#endif

    // --- Page 4 ---

    unsigned char usarea[128];              // = $0480          128 bytes reserved for application

    // --- Page 5 ---

    unsigned char _spare_7[126];            // = $0500-$057D    reserved for FP package / unused
    unsigned char lbpr1;                    // = $057E          LBUFF PREFIX 1
    unsigned char lbpr2;                    // = $057F          LBUFF PREFIX 2
    unsigned char lbuff[128];               // = $0580-$05FF    128-byte line buffer
};


/* Define a structure with the zero page atari basic register offsets */

struct __basic {
    void*         lowmem;                   // = $80/$81        POINTER TO BASIC'S LOW MEMORY
    void*         vntp;                     // = $82/$83        BEGINNING ADDRESS OF THE VARIABLE NAME TABLE
    void*         vntd;                     // = $84/$85        POINTER TO THE ENDING ADDRESS OF THE VARIABLE NAME TABLE PLUS ONE
    void*         vvtp;                     // = $86/$87        ADDRESS FOR THE VARIABLE VALUE TABLE
    void*         stmtab;                   // = $88/$89        ADDRESS OF THE STATEMENT TABLE
    void*         stmcur;                   // = $8A/$8B        CURRENT BASIC STATEMENT POINTER
    void*         starp;                    // = $8C/$8D        ADDRESS FOR THE STRING AND ARRAY TABLE
    void*         runstk;                   // = $8E/$8F        ADDRESS OF THE RUNTIME STACK
    void*         memtop;                   // = $90/$91        POINTER TO THE TOP OF BASIC MEMORY
    
    unsigned char   _internal_1[0xBA-0x91-1];  // INTERNAL DATA

    unsigned int  stopln;                   // = $BA/$BB        LINE WHERE A PROGRAM WAS STOPPED

    unsigned char   _internal_2[0xC3-0xBB-1];   // INTERNAL DATA

    unsigned char errsav;                   // = $C3            NUMBER OF THE ERROR CODE

    unsigned char   _internal_3[0xC9-0xC3-1];   // INTERNAL DATA

    unsigned char ptabw;                    // = $C9            NUMBER OF COLUMNS BETWEEN TAB STOPS
    unsigned char loadflg;                  // = $CA            LIST PROTECTION

    unsigned char   _internal_4[0xD4-0xCA-1];   // INTERNAL DATA

    unsigned int  binint;                   // = $D4/$D5        USR-CALL RETURN VALUE
};

#endif
