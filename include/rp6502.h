/*****************************************************************************/
/*                                                                           */
/*                                 rp6502.h                                  */
/*                                                                           */
/*                            Picocomputer 6502                              */
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

#ifndef _RP6502_H
#define _RP6502_H

/* RP6502 VIA $FFD0-$FFDF */

#include <_6522.h>
#define VIA (*(volatile struct __6522 *)0xFFD0)

/* RP6502 RIA $FFE0-$FFF9 */

struct __RP6502
{
    const unsigned char ready;
    unsigned char tx;
    const unsigned char rx;
    const unsigned char vsync;
    unsigned char rw0;
    signed char step0;
    unsigned int addr0;
    unsigned char rw1;
    signed char step1;
    unsigned int addr1;
    unsigned char xstack;
    unsigned int errno_;
    unsigned char op;
    unsigned char irq;
    const unsigned char spin;
    const unsigned char busy;
    const unsigned char lda;
    unsigned char a;
    const unsigned char ldx;
    unsigned char x;
    const unsigned char rts;
    unsigned int sreg;
};
#define RIA (*(volatile struct __RP6502 *)0xFFE0)

/* OS operation numbers */

#define RIA_OP_EXIT 0xFF
#define RIA_OP_DROP_XSTACK 0x00
#define RIA_OP_XREG 0x01
#define RIA_OP_ARGV 0x08
#define RIA_OP_EXEC 0x09
#define RIA_OP_ATTR_GET 0x0A
#define RIA_OP_ATTR_SET 0x0B
#define RIA_OP_OPEN 0x14
#define RIA_OP_CLOSE 0x15
#define RIA_OP_READ_XSTACK 0x16
#define RIA_OP_READ_XRAM 0x17
#define RIA_OP_WRITE_XSTACK 0x18
#define RIA_OP_WRITE_XRAM 0x19
#define RIA_OP_LSEEK 0x1A
#define RIA_OP_LSEEK_CC65 0x1A
#define RIA_OP_UNLINK 0x1B
#define RIA_OP_RENAME 0x1C
#define RIA_OP_LSEEK_LLVM 0x1D
#define RIA_OP_SYNCFS 0x1E
#define RIA_OP_STAT 0x1F
#define RIA_OP_OPENDIR 0x20
#define RIA_OP_READDIR 0x21
#define RIA_OP_CLOSEDIR 0x22
#define RIA_OP_TELLDIR 0x23
#define RIA_OP_SEEKDIR 0x24
#define RIA_OP_REWINDDIR 0x25
#define RIA_OP_CHMOD 0x26
#define RIA_OP_UTIME 0x27
#define RIA_OP_MKDIR 0x28
#define RIA_OP_CHDIR 0x29
#define RIA_OP_CHDRIVE 0x2A
#define RIA_OP_GETCWD 0x2B
#define RIA_OP_SETLABEL 0x2C
#define RIA_OP_GETLABEL 0x2D
#define RIA_OP_GETFREE 0x2E
#define RIA_OP_RLN_LASTKEY 0x30
#define RIA_OP_RLN_PEEK 0x31
#define RIA_OP_RLN_POKE 0x32
#define RIA_OP_GMTIME 0x3A
#define RIA_OP_LOCALTIME 0x3B
#define RIA_OP_MKTIME 0x3C
#define RIA_OP_STRFTIME 0x3D
#define RIA_OP_TIME_SET 0x3E
#define RIA_OP_TIME_GET 0x3F

/* RIA attribute IDs */

#define RIA_ATTR_ERRNO_OPT 0x00
#define RIA_ATTR_PHI2_KHZ 0x01
#define RIA_ATTR_CODE_PAGE 0x02
#define RIA_ATTR_RLN_LENGTH 0x03
#define RIA_ATTR_LRAND 0x04
#define RIA_ATTR_BEL 0x05
#define RIA_ATTR_LAUNCHER 0x06
#define RIA_ATTR_EXIT_CODE 0x07
#define RIA_ATTR_SIGINT 0x08
#define RIA_ATTR_RLN_CAPS 0x09
#define RIA_ATTR_RLN_WIDTH 0x0A
#define RIA_ATTR_RLN_HEIGHT 0x0B
#define RIA_ATTR_RLN_SUPPRESS_NL 0x0C
#define RIA_ATTR_CLK_RUN_MS 0x10
#define RIA_ATTR_CLK_RUN_CS 0x11
#define RIA_ATTR_CLK_RUN_DS 0x12
#define RIA_ATTR_CLK_RUN_S 0x13

/* C API for the RIA. */

typedef struct {
    unsigned long fsize;
    unsigned fdate;
    unsigned ftime;
    unsigned crdate;
    unsigned crtime;
    unsigned char fattrib;
    char altname[12 + 1];
    char fname[255 + 1];
} f_stat_t;

int ria_spin (void);
void ria_drop (void);
unsigned char ria_vsync (void);
unsigned char ria_irq_read (void);
void __fastcall__ ria_irq_write (unsigned char mask);
int __fastcall__ ria_execv (const char* path, char* const argv[]);
int ria_execl (const char* path, ...);
long __fastcall__ ria_attr_get (unsigned char id);
int __fastcall__ ria_attr_set (long val, unsigned char id);
int __fastcall__ read_xstack (void* buf, unsigned count, int fildes);
int __fastcall__ read_xram (unsigned buf, unsigned count, int fildes);
int __fastcall__ write_xstack (const void* buf, unsigned count, int fildes);
int __fastcall__ write_xram (unsigned buf, unsigned count, int fildes);
long __fastcall__ f_lseek (long offset, int whence, int fildes);
int __fastcall__ f_stat (const char* path, f_stat_t* dirent);
int __fastcall__ f_opendir (const char* name);
int __fastcall__ f_readdir (f_stat_t* dirent, int dirdes);
int __fastcall__ f_closedir (int dirdes);
long __fastcall__ f_telldir (int dirdes);
int __fastcall__ f_seekdir (long offs, int dirdes);
int __fastcall__ f_rewinddir (int dirdes);
int __fastcall__ f_chmod (const char* path, unsigned char attr, unsigned char mask);
int __fastcall__ f_utime (const char* path, unsigned fdate, unsigned ftime, unsigned crdate, unsigned crtime);
int __fastcall__ f_mkdir (const char* name);
int __fastcall__ f_chdrive (const char* name);
int __fastcall__ f_getcwd (char* name, int size);
int __fastcall__ f_setlabel (const char* name);
int __fastcall__ f_getlabel (const char* path, char* label);
int __fastcall__ f_getfree (const char* name, unsigned long* free, unsigned long* total);
int __fastcall__ ria_rln_lastkey (char* key, unsigned char* action);
int __fastcall__ ria_rln_peek (char* peek, unsigned char* pos);
int __fastcall__ ria_rln_poke (const char* poke);
int __fastcall__ time_set (unsigned long time);

/* Extended memory */

int xreg (char device, char channel, unsigned char address, ...);
int xregn (char device, char channel, unsigned char address, unsigned count,
    ...);

void __fastcall__ xram0_read (void* dest, unsigned src, unsigned count);
void __fastcall__ xram1_read (void* dest, unsigned src, unsigned count);
void __fastcall__ xram0_write (unsigned dest, const void* src, unsigned count);
void __fastcall__ xram1_write (unsigned dest, const void* src, unsigned count);
void __fastcall__ xram0_set (unsigned dest, unsigned char val, unsigned count);
void __fastcall__ xram1_set (unsigned dest, unsigned char val, unsigned count);
void __fastcall__ xram_move (unsigned dest, unsigned src, unsigned count);

unsigned char __fastcall__ xram0_peek8 (unsigned addr);
unsigned char __fastcall__ xram1_peek8 (unsigned addr);
unsigned __fastcall__ xram0_peek16 (unsigned addr);
unsigned __fastcall__ xram1_peek16 (unsigned addr);
void __fastcall__ _xram0_poke8 (unsigned char val, unsigned addr);
#define xram0_poke8(addr, val) _xram0_poke8 (val, addr)
void __fastcall__ _xram1_poke8 (unsigned char val, unsigned addr);
#define xram1_poke8(addr, val) _xram1_poke8 (val, addr)
void __fastcall__ _xram0_poke16 (unsigned val, unsigned addr);
#define xram0_poke16(addr, val) _xram0_poke16 (val, addr)
void __fastcall__ _xram1_poke16 (unsigned val, unsigned addr);
#define xram1_poke16(addr, val) _xram1_poke16 (val, addr)

#endif /* _RP6502_H */
