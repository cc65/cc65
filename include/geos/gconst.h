/*
  GEOS constants

  reassembled by Maciej 'YTM/Elysium' Witkowiak
*/

/* Here are constants which didn't fit into any other category... */

#ifndef _GCONST_H
#define _GCONST_H

#ifndef NULL
#define NULL            ((void *) 0)
#endif
#define FALSE           0
#define TRUE            0xff
#define MOUSE_SPRNUM    0
#define DISK_DRV_LGH    0x0d80

/* drivetypes */
#define DRV_NULL        0
#define DRV_1541        1
#define DRV_1571        2
#define DRV_1581        3
#define DRV_NETWORK     15

/* various disk constants */
#define REL_FILE_NUM    9
#define CMND_FILE_NUM   15
#define MAX_CMND_STR    32
#define DIR_1581_TRACK  40
#define DIR_ACC_CHAN    13
#define DIR_TRACK       18
#define N_TRACKS        35
#define DK_NM_ID_LEN    18
#define TRACK           9
#define SECTOR          12
#define TOTAL_BLOCKS    664

/* offset to something */
#define OFF_INDEX_PTR   1

/* values for CPU_DATA memory config - C64 */
#define IO_IN           0x35
#define KRNL_IO_IN      0x36
#define KRNL_BAS_IO_IN  0x37

/* values for MMU config - C128 */
#define CIOIN           0x7E
#define CRAM64K         0x7F
#define CKRNLBASIOIN    0x40
#define CKRNLIOIN       0x4E

/* alarmSetFlag */
#define ALARMMASK       4

#define CLR_SAVE        0x40
#define CONSTRAINED     0x40
#define UN_CONSTRAINED  0
#define FG_SAVE         0x80

#define FUTURE1         7
#define FUTURE2         8
#define FUTURE3         9
#define FUTURE4         10
#define USELAST         127
#define SHORTCUT        128

#endif
