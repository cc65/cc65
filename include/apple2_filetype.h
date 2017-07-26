/*****************************************************************************/
/*                                                                           */
/*                             apple2_filetype.h                             */
/*                                                                           */
/*                       Apple ][ file type definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000  Kevin Ruland, <kevin@rodin.wustl.edu>                           */
/* (C) 2003  Ullrich von Bassewitz, <uz@cc65.org>                            */
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



#ifndef _APPLE2_FILETYPE_H
#define _APPLE2_FILETYPE_H



/* Check for errors */
#if !defined(__APPLE2__)
#  error This module may only be used when compiling for the Apple ][!
#endif


/* ProDOS general file types */
#define PRODOS_FILE_TYPE_UNK    0x00    /* Unknown                      */
#define PRODOS_FILE_TYPE_BAD    0x01    /* Bad blocks                   */
#define PRODOS_FILE_TYPE_PCD    0x02    /* Pascal code                  */
#define PRODOS_FILE_TYPE_PTX    0x03    /* Pascal text                  */
#define PRODOS_FILE_TYPE_TXT    0x04    /* ASCII text                   */
#define PRODOS_FILE_TYPE_PDA    0x05    /* Pascal data                  */
#define PRODOS_FILE_TYPE_BIN    0x06    /* Binary                       */
#define PRODOS_FILE_TYPE_FNT    0x07    /* Apple III font               */
#define PRODOS_FILE_TYPE_FOT    0x08    /* Hi-res, dbl hi-res graphics  */
#define PRODOS_FILE_TYPE_BA3    0x09    /* Apple III BASIC program      */
#define PRODOS_FILE_TYPE_DA3    0x09    /* Apple III BASIC data         */
#define PRODOS_FILE_TYPE_WPF    0x0A    /* Generic word processing      */
#define PRODOS_FILE_TYPE_SOS    0x0B    /* SOS system                   */
#define PRODOS_FILE_TYPE_DIR    0x0F    /* ProDOS directory             */

/* ProDOS productivity file types */
#define PRODOS_FILE_TYPE_RPD    0x10    /* RPS data                     */
#define PRODOS_FILE_TYPE_RPI    0x11    /* RPS index                    */
#define PRODOS_FILE_TYPE_AFD    0x12    /* AppleFile discard            */
#define PRODOS_FILE_TYPE_AFM    0x13    /* AppleFile model              */
#define PRODOS_FILE_TYPE_AFR    0x14    /* AppleFile report             */
#define PRODOS_FILE_TYPE_SCL    0x15    /* Screen library               */
#define PRODOS_FILE_TYPE_PFS    0x16    /* PFS document                 */
#define PRODOS_FILE_TYPE_ADB    0x19    /* AppleWorks database          */
#define PRODOS_FILE_TYPE_AWP    0x1A    /* AppleWorks word processing   */
#define PRODOS_FILE_TYPE_ASP    0x1B    /* AppleWorks spreadsheet       */

/* ProDOS code file types */
#define PRODOS_FILE_TYPE_TDM    0x20    /* Desktop Manager              */
#define PRODOS_FILE_TYPE_IPS    0x21    /* Instant Pascal source        */
#define PRODOS_FILE_TYPE_UPV    0x22    /* USCD Pascal volume           */
#define PRODOS_FILE_TYPE_3SD    0x29    /* SOS directory                */
#define PRODOS_FILE_TYPE_8SC    0x2A    /* Source code                  */
#define PRODOS_FILE_TYPE_8OB    0x2B    /* Object code                  */
#define PRODOS_FILE_TYPE_8IC    0x2C    /* Interpreted code             */
#define PRODOS_FILE_TYPE_8LD    0x2D    /* Language data                */
#define PRODOS_FILE_TYPE_P8C    0x2E    /* ProDOS 8 code module         */

/* ProDOS miscellaneous file types */
#define PRODOS_FILE_TYPE_OCR    0x41    /* Optical char recognition     */
#define PRODOS_FILE_TYPE_FTD    0x42    /* File type definitions        */

/* ProDOS Apple IIgs general file types */
#define PRODOS_FILE_TYPE_GWP    0x50    /* Apple IIgs word processing   */
#define PRODOS_FILE_TYPE_GSS    0x51    /* Apple IIgs spreadsheet       */
#define PRODOS_FILE_TYPE_GDB    0x52    /* Apple IIgs database          */
#define PRODOS_FILE_TYPE_DRW    0x53    /* Object oriented graphics     */
#define PRODOS_FILE_TYPE_GDP    0x54    /* Apple IIgs desktop publish   */
#define PRODOS_FILE_TYPE_HMD    0x55    /* HyperMedia                   */
#define PRODOS_FILE_TYPE_EDU    0x56    /* Educational program data     */
#define PRODOS_FILE_TYPE_STN    0x57    /* Stationary                   */
#define PRODOS_FILE_TYPE_HLP    0x58    /* Help                         */
#define PRODOS_FILE_TYPE_COM    0x59    /* Communications               */
#define PRODOS_FILE_TYPE_CFG    0x5A    /* Configuration                */
#define PRODOS_FILE_TYPE_ANM    0x5B    /* Animation                    */
#define PRODOS_FILE_TYPE_MUM    0x5C    /* Multimedia                   */
#define PRODOS_FILE_TYPE_ENT    0x5D    /* Entertainment                */
#define PRODOS_FILE_TYPE_DVU    0x5E    /* Development utility          */

/* ProDOS PC Transporter file types */
#define PRODOS_FILE_TYPE_PRE    0x60    /* PC pre-boot                  */
#define PRODOS_FILE_TYPE_BIO    0x6B    /* PC BIOS                      */
#define PRODOS_FILE_TYPE_NCF    0x66    /* ProDOS File Nav command file */
#define PRODOS_FILE_TYPE_DVR    0x6D    /* PC driver                    */
#define PRODOS_FILE_TYPE_PRE2   0x6E    /* PC pre-boot                  */
#define PRODOS_FILE_TYPE_HDV    0x6F    /* PC hard disk image           */

/* ProDOS Kreative Software file types */
#define PRODOS_FILE_TYPE_SN2    0x70    /* Sabine's Notebook 2.0        */
#define PRODOS_FILE_TYPE_KMT    0x71
#define PRODOS_FILE_TYPE_DSR    0x72
#define PRODOS_FILE_TYPE_BAN    0x73
#define PRODOS_FILE_TYPE_CG7    0x74
#define PRODOS_FILE_TYPE_TNJ    0x75
#define PRODOS_FILE_TYPE_SA7    0x76
#define PRODOS_FILE_TYPE_KES    0x77
#define PRODOS_FILE_TYPE_JAP    0x78
#define PRODOS_FILE_TYPE_CSL    0x79
#define PRODOS_FILE_TYPE_TME    0x7A
#define PRODOS_FILE_TYPE_TLB    0x7B
#define PRODOS_FILE_TYPE_MR7    0x7C
#define PRODOS_FILE_TYPE_MLR    0x7D    /* Mika City                    */
#define PRODOS_FILE_TYPE_MMM    0x7E
#define PRODOS_FILE_TYPE_JCP    0x7F

/* ProDOS GEOS file types */
#define PRODOS_FILE_TYPE_GES    0x80    /* GEOS system file             */
#define PRODOS_FILE_TYPE_GEA    0x81    /* GEOS desk accessory          */
#define PRODOS_FILE_TYPE_GEO    0x82    /* GEOS application             */
#define PRODOS_FILE_TYPE_GED    0x83    /* GEOS document                */
#define PRODOS_FILE_TYPE_GEF    0x84    /* GEOS font                    */
#define PRODOS_FILE_TYPE_GEP    0x85    /* GEOS printer driver          */
#define PRODOS_FILE_TYPE_GEI    0x86    /* GEOS input driver            */
#define PRODOS_FILE_TYPE_GEX    0x87    /* GEOS auxiliary driver        */
#define PRODOS_FILE_TYPE_GEV    0x89    /* GEOS swap file               */
#define PRODOS_FILE_TYPE_GEC    0x8B    /* GEOS clock driver            */
#define PRODOS_FILE_TYPE_GEK    0x8C    /* GEOS interface card driver   */
#define PRODOS_FILE_TYPE_GEW    0x8D    /* GEOS formatting data         */

/* ProDOS Apple IIgs BASIC file types */
#define PRODOS_FILE_TYPE_WP     0xA0    /* WordPerfect                  */
#define PRODOS_FILE_TYPE_GSB    0xAB    /* Apple IIgs BASIC program     */
#define PRODOS_FILE_TYPE_TDF    0xAB    /* Apple IIgs BASIC TDF         */
#define PRODOS_FILE_TYPE_BDF    0xAB    /* Apple IIgs BASIC data        */

/* ProDOS Apple IIgs system file types */
#define PRODOS_FILE_TYPE_SRC    0xB0    /* Apple IIgs source code       */
#define PRODOS_FILE_TYPE_OBJ    0xB1    /* Apple IIgs object code       */
#define PRODOS_FILE_TYPE_LIB    0xB2    /* Apple IIgs library           */
#define PRODOS_FILE_TYPE_S16    0xB3    /* Apple IIgs application pgm   */
#define PRODOS_FILE_TYPE_RTL    0xB4    /* Apple IIgs runtime library   */
#define PRODOS_FILE_TYPE_EXE    0xB5    /* Apple IIgs shell script      */
#define PRODOS_FILE_TYPE_PIF    0xB6    /* Apple IIgs permanent init    */
#define PRODOS_FILE_TYPE_TIF    0xB7    /* Apple IIgs temporary init    */
#define PRODOS_FILE_TYPE_NDA    0xB8    /* Apple IIgs new desk accesry  */
#define PRODOS_FILE_TYPE_CDA    0xB9    /* Apple IIgs classic desk aces */
#define PRODOS_FILE_TYPE_TOL    0xBA    /* Apple IIgs tool              */
#define PRODOS_FILE_TYPE_DRV    0xBB    /* Apple IIgs device driver     */
#define PRODOS_FILE_TYPE_LDF    0xBC    /* Apple IIgs generic load file */
#define PRODOS_FILE_TYPE_FST    0xBD    /* Apple IIgs file sys translat */
#define PRODOS_FILE_TYPE_DOC    0xBF    /* Apple IIgs document          */

/* ProDOS graphics file types */
#define PRODOS_FILE_TYPE_PNT    0xC0    /* Apple IIgs packed sup hi-res */
#define PRODOS_FILE_TYPE_PIC    0xC1    /* Apple IIgs super hi-res      */
#define PRODOS_FILE_TYPE_ANI    0xC2    /* PaintWorks animation         */
#define PRODOS_FILE_TYPE_PAL    0xC3    /* PaintWorks palette           */
#define PRODOS_FILE_TYPE_OOG    0xC5    /* Object-oriented graphics     */
#define PRODOS_FILE_TYPE_SCR    0xC6    /* Script                       */
#define PRODOS_FILE_TYPE_CDV    0xC7    /* Apple IIgs control panel     */
#define PRODOS_FILE_TYPE_FON    0xC8    /* Apple IIgs font              */
#define PRODOS_FILE_TYPE_FND    0xC9    /* Apple IIgs Finder data       */
#define PRODOS_FILE_TYPE_ICN    0xCA    /* Apple IIgs icon              */

/* ProDOS audio file types */
#define PRODOS_FILE_TYPE_MUS    0xD5    /* Music                        */
#define PRODOS_FILE_TYPE_INS    0xD6    /* Instrument                   */
#define PRODOS_FILE_TYPE_MID    0xD7    /* MIDI                         */
#define PRODOS_FILE_TYPE_SND    0xD8    /* Apple IIgs audio             */
#define PRODOS_FILE_TYPE_DBM    0xDB    /* DB master document           */

/* ProDOS miscellaneous file types */
#define PRODOS_FILE_TYPE_LBR    0xE0    /* Archive                      */
#define PRODOS_FILE_TYPE_ATK    0xE2    /* AppleTalk data               */
#define PRODOS_FILE_TYPE_R16    0xEE    /* EDASM 816 relocatable code   */
#define PRODOS_FILE_TYPE_PAR    0xEF    /* Pascal area                  */

/* ProDOS system file types */
#define PRODOS_FILE_TYPE_CMD    0xF0    /* ProDOS command file          */
#define PRODOS_T_OVL    0xF1    /* User defined 1               */
#define PRODOS_FILE_TYPE_UD2    0xF2    /* User defined 2               */
#define PRODOS_FILE_TYPE_UD3    0xF3    /* User defined 3               */
#define PRODOS_FILE_TYPE_UD4    0xF4    /* User defined 4               */
#define PRODOS_FILE_TYPE_BAT    0xF5    /* User defined 5               */
#define PRODOS_FILE_TYPE_UD6    0xF6    /* User defined 6               */
#define PRODOS_FILE_TYPE_UD7    0xF7    /* User defined 7               */
#define PRODOS_FILE_TYPE_PRG    0xF8    /* User defined 8               */
#define PRODOS_FILE_TYPE_P16    0xF9    /* ProDOS-16 system file        */
#define PRODOS_FILE_TYPE_INT    0xFA    /* Integer BASIC program        */
#define PRODOS_FILE_TYPE_IVR    0xFB    /* Integer BASIC variables      */
#define PRODOS_FILE_TYPE_BAS    0xFC    /* Applesoft BASIC program      */
#define PRODOS_FILE_TYPE_VAR    0xFD    /* Applesoft BASIC variables    */
#define PRODOS_FILE_TYPE_REL    0xFE    /* EDASM relocatable code       */
#define PRODOS_FILE_TYPE_SYS    0xFF    /* ProDOS-8 system file         */

/* The auxiliary type of a text file specifies its record length.
** A record length of 0 indicates a sequential text file, which is
** equivalent to text files of other operating systems like MacOS
** or Windows, except that lines are delimited by carriage returns
** only. An auxiliary type value greater than 0 for a text file,
** which is the record length, indicates a random access text file
** with fixed-length lines.
*/
#define PRODOS_AUX_T_TXT_SEQ            0x0000  /* Sequential text  */

/* 8IC auxiliary type */
#define PRODOS_AUX_T_8IC_APEX_PGM       0x8003  /* Apex program     */

/* GWP auxiliary types */
#define PRODOS_AUX_T_GWP_TEACH          0x5445  /* Teach            */
#define PRODOS_AUX_T_GWP_DELUXEWRITE    0x8001  /* DeluxeWrite      */
#define PRODOS_AUX_T_GWP_APPLEWORKS_GS  0x8010  /* AppleWorks GS    */

/* GSS auxiliary type */
#define PRODOS_AUX_T_GSS_APPLEWORKS_GS  0x8010  /* AppleWorks GS    */

/* GDB auxiliary types */
#define PRODOS_GDB_AUX_TYPE_APLWRKSGS   0x8010  /* AppleWorks GS DB */
#define PRODOS_GDB_AUX_TYPE_AWGS_TMPL   0x8011  /* AWGS template    */
#define PRODOS_GDB_AUX_TYPE_GSAS        0x8013

/* DRW auxiliary type */
#define PRODOS_DRW_AUX_TYPE_APLWRKSGS   0x8013  /* AWGS O-O graphics    */

/* GDP auxiliary types */
#define PRODOS_GDP_AUX_TYPE_GRFCWRTR    0x8002  /* AWGS GraphicWriter   */
#define PRODOS_GDP_AUX_TYPE_APLWRKSGS   0x8010  /* AWGS                 */

/* HMD auxiliary types */
#define PRODOS_HMD_AUX_TYPE_HYPRCRDGS   0x0001  /* HyperCard GS         */
#define PRODOS_HMD_AUX_TYPE_TUTORTECH   0x8001  /* Tutor-Tech           */
#define PRODOS_HMD_AUX_TYPE_HYPRSTDIO   0x8002  /* HyperStudio          */
#define PRODOS_HMD_AUX_TYPE_NEXUS       0x8003  /* Nexus                */

/* COM auxiliary type */
#define PRODOS_COM_AUX_TYPE_APLWRKSGS   0x8003  /* AppleWorks GS        */

/* End of apple2_filetypes.h */
#endif
