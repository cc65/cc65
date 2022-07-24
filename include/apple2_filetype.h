/*****************************************************************************/
/*                                                                           */
/*                             apple2_filetype.h                             */
/*                                                                           */
/*                       Apple ][ file type definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017  Bill Chatfield, <bill_chatfield@yahoo.com>                      */
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



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* ProDOS general file types */
#define PRODOS_T_UNK    0x00    /* Unknown                      */
#define PRODOS_T_BAD    0x01    /* Bad blocks                   */
#define PRODOS_T_PCD    0x02    /* Pascal code                  */
#define PRODOS_T_PTX    0x03    /* Pascal text                  */
#define PRODOS_T_TXT    0x04    /* ASCII text                   */
#define PRODOS_T_PDA    0x05    /* Pascal data                  */
#define PRODOS_T_BIN    0x06    /* Binary                       */
#define PRODOS_T_FNT    0x07    /* Apple III font               */
#define PRODOS_T_FOT    0x08    /* Hi-res, dbl hi-res graphics  */
#define PRODOS_T_BA3    0x09    /* Apple III BASIC program      */
#define PRODOS_T_DA3    0x09    /* Apple III BASIC data         */
#define PRODOS_T_WPF    0x0A    /* Generic word processing      */
#define PRODOS_T_SOS    0x0B    /* SOS system                   */
#define PRODOS_T_DIR    0x0F    /* ProDOS directory             */

/* ProDOS productivity file types */
#define PRODOS_T_RPD    0x10    /* RPS data                     */
#define PRODOS_T_RPI    0x11    /* RPS index                    */
#define PRODOS_T_AFD    0x12    /* AppleFile discard            */
#define PRODOS_T_AFM    0x13    /* AppleFile model              */
#define PRODOS_T_AFR    0x14    /* AppleFile report             */
#define PRODOS_T_SCL    0x15    /* Screen library               */
#define PRODOS_T_PFS    0x16    /* PFS document                 */
#define PRODOS_T_ADB    0x19    /* AppleWorks database          */
#define PRODOS_T_AWP    0x1A    /* AppleWorks word processing   */
#define PRODOS_T_ASP    0x1B    /* AppleWorks spreadsheet       */

/* ProDOS code file types */
#define PRODOS_T_TDM    0x20    /* Desktop Manager              */
#define PRODOS_T_IPS    0x21    /* Instant Pascal source        */
#define PRODOS_T_UPV    0x22    /* USCD Pascal volume           */
#define PRODOS_T_3SD    0x29    /* SOS directory                */
#define PRODOS_T_8SC    0x2A    /* Source code                  */
#define PRODOS_T_8OB    0x2B    /* Object code                  */
#define PRODOS_T_8IC    0x2C    /* Interpreted code             */
#define PRODOS_T_8LD    0x2D    /* Language data                */
#define PRODOS_T_P8C    0x2E    /* ProDOS 8 code module         */

/* ProDOS miscellaneous file types */
#define PRODOS_T_OCR    0x41    /* Optical char recognition     */
#define PRODOS_T_FTD    0x42    /* File type definitions        */

/* ProDOS Apple IIgs general file types */
#define PRODOS_T_GWP    0x50    /* Apple IIgs word processing   */
#define PRODOS_T_GSS    0x51    /* Apple IIgs spreadsheet       */
#define PRODOS_T_GDB    0x52    /* Apple IIgs database          */
#define PRODOS_T_DRW    0x53    /* Object oriented graphics     */
#define PRODOS_T_GDP    0x54    /* Apple IIgs desktop publish   */
#define PRODOS_T_HMD    0x55    /* HyperMedia                   */
#define PRODOS_T_EDU    0x56    /* Educational program data     */
#define PRODOS_T_STN    0x57    /* Stationary                   */
#define PRODOS_T_HLP    0x58    /* Help                         */
#define PRODOS_T_COM    0x59    /* Communications               */
#define PRODOS_T_CFG    0x5A    /* Configuration                */
#define PRODOS_T_ANM    0x5B    /* Animation                    */
#define PRODOS_T_MUM    0x5C    /* Multimedia                   */
#define PRODOS_T_ENT    0x5D    /* Entertainment                */
#define PRODOS_T_DVU    0x5E    /* Development utility          */

/* ProDOS PC Transporter file types */
#define PRODOS_T_PRE    0x60    /* PC pre-boot                  */
#define PRODOS_T_BIO    0x6B    /* PC BIOS                      */
#define PRODOS_T_NCF    0x66    /* ProDOS File Nav command file */
#define PRODOS_T_DVR    0x6D    /* PC driver                    */
#define PRODOS_T_PRE2   0x6E    /* PC pre-boot                  */
#define PRODOS_T_HDV    0x6F    /* PC hard disk image           */

/* ProDOS Kreative Software file types */
#define PRODOS_T_SN2    0x70    /* Sabine's Notebook 2.0        */
#define PRODOS_T_KMT    0x71
#define PRODOS_T_DSR    0x72
#define PRODOS_T_BAN    0x73
#define PRODOS_T_CG7    0x74
#define PRODOS_T_TNJ    0x75
#define PRODOS_T_SA7    0x76
#define PRODOS_T_KES    0x77
#define PRODOS_T_JAP    0x78
#define PRODOS_T_CSL    0x79
#define PRODOS_T_TME    0x7A
#define PRODOS_T_TLB    0x7B
#define PRODOS_T_MR7    0x7C
#define PRODOS_T_MLR    0x7D    /* Mika City                    */
#define PRODOS_T_MMM    0x7E
#define PRODOS_T_JCP    0x7F

/* ProDOS GEOS file types */
#define PRODOS_T_GES    0x80    /* GEOS system file             */
#define PRODOS_T_GEA    0x81    /* GEOS desk accessory          */
#define PRODOS_T_GEO    0x82    /* GEOS application             */
#define PRODOS_T_GED    0x83    /* GEOS document                */
#define PRODOS_T_GEF    0x84    /* GEOS font                    */
#define PRODOS_T_GEP    0x85    /* GEOS printer driver          */
#define PRODOS_T_GEI    0x86    /* GEOS input driver            */
#define PRODOS_T_GEX    0x87    /* GEOS auxiliary driver        */
#define PRODOS_T_GEV    0x89    /* GEOS swap file               */
#define PRODOS_T_GEC    0x8B    /* GEOS clock driver            */
#define PRODOS_T_GEK    0x8C    /* GEOS interface card driver   */
#define PRODOS_T_GEW    0x8D    /* GEOS formatting data         */

/* ProDOS Apple IIgs BASIC file types */
#define PRODOS_T_WP     0xA0    /* WordPerfect                  */
#define PRODOS_T_GSB    0xAB    /* Apple IIgs BASIC program     */
#define PRODOS_T_TDF    0xAB    /* Apple IIgs BASIC TDF         */
#define PRODOS_T_BDF    0xAB    /* Apple IIgs BASIC data        */

/* ProDOS Apple IIgs system file types */
#define PRODOS_T_SRC    0xB0    /* Apple IIgs source code       */
#define PRODOS_T_OBJ    0xB1    /* Apple IIgs object code       */
#define PRODOS_T_LIB    0xB2    /* Apple IIgs library           */
#define PRODOS_T_S16    0xB3    /* Apple IIgs application pgm   */
#define PRODOS_T_RTL    0xB4    /* Apple IIgs runtime library   */
#define PRODOS_T_EXE    0xB5    /* Apple IIgs shell script      */
#define PRODOS_T_PIF    0xB6    /* Apple IIgs permanent init    */
#define PRODOS_T_TIF    0xB7    /* Apple IIgs temporary init    */
#define PRODOS_T_NDA    0xB8    /* Apple IIgs new desk accesry  */
#define PRODOS_T_CDA    0xB9    /* Apple IIgs classic desk aces */
#define PRODOS_T_TOL    0xBA    /* Apple IIgs tool              */
#define PRODOS_T_DRV    0xBB    /* Apple IIgs device driver     */
#define PRODOS_T_LDF    0xBC    /* Apple IIgs generic load file */
#define PRODOS_T_FST    0xBD    /* Apple IIgs file sys translat */
#define PRODOS_T_DOC    0xBF    /* Apple IIgs document          */

/* ProDOS graphics file types */
#define PRODOS_T_PNT    0xC0    /* Apple IIgs packed sup hi-res */
#define PRODOS_T_PIC    0xC1    /* Apple IIgs super hi-res      */
#define PRODOS_T_ANI    0xC2    /* PaintWorks animation         */
#define PRODOS_T_PAL    0xC3    /* PaintWorks palette           */
#define PRODOS_T_OOG    0xC5    /* Object-oriented graphics     */
#define PRODOS_T_SCR    0xC6    /* Script                       */
#define PRODOS_T_CDV    0xC7    /* Apple IIgs control panel     */
#define PRODOS_T_FON    0xC8    /* Apple IIgs font              */
#define PRODOS_T_FND    0xC9    /* Apple IIgs Finder data       */
#define PRODOS_T_ICN    0xCA    /* Apple IIgs icon              */

/* ProDOS audio file types */
#define PRODOS_T_MUS    0xD5    /* Music                        */
#define PRODOS_T_INS    0xD6    /* Instrument                   */
#define PRODOS_T_MID    0xD7    /* MIDI                         */
#define PRODOS_T_SND    0xD8    /* Apple IIgs audio             */
#define PRODOS_T_DBM    0xDB    /* DB master document           */

/* ProDOS miscellaneous file types */
#define PRODOS_T_LBR    0xE0    /* Archive                      */
#define PRODOS_T_ATK    0xE2    /* AppleTalk data               */
#define PRODOS_T_R16    0xEE    /* EDASM 816 relocatable code   */
#define PRODOS_T_PAR    0xEF    /* Pascal area                  */

/* ProDOS system file types */
#define PRODOS_T_CMD    0xF0    /* ProDOS command file          */
#define PRODOS_T_OVL    0xF1    /* User defined 1               */
#define PRODOS_T_UD2    0xF2    /* User defined 2               */
#define PRODOS_T_UD3    0xF3    /* User defined 3               */
#define PRODOS_T_UD4    0xF4    /* User defined 4               */
#define PRODOS_T_BAT    0xF5    /* User defined 5               */
#define PRODOS_T_UD6    0xF6    /* User defined 6               */
#define PRODOS_T_UD7    0xF7    /* User defined 7               */
#define PRODOS_T_PRG    0xF8    /* User defined 8               */
#define PRODOS_T_P16    0xF9    /* ProDOS-16 system file        */
#define PRODOS_T_INT    0xFA    /* Integer BASIC program        */
#define PRODOS_T_IVR    0xFB    /* Integer BASIC variables      */
#define PRODOS_T_BAS    0xFC    /* Applesoft BASIC program      */
#define PRODOS_T_VAR    0xFD    /* Applesoft BASIC variables    */
#define PRODOS_T_REL    0xFE    /* EDASM relocatable code       */
#define PRODOS_T_SYS    0xFF    /* ProDOS-8 system file         */

/* The auxiliary type of a text file specifies its record length.
** A record length of 0 indicates a sequential text file, which is
** equivalent to text files of other operating systems like MacOS
** or Windows, except that lines are delimited by carriage returns
** only. An auxiliary type value greater than 0 for a text file,
** which is the record length, indicates a random access text file
** with fixed-length lines.
*/
#define PRODOS_AUX_T_TXT_SEQ            0x0000  /* Sequential text          */

/* 8IC auxiliary types */
#define PRODOS_AUX_T_8IC_APEX_PGM       0x8003  /* Apex program             */

/* GWP auxiliary types */
#define PRODOS_AUX_T_GWP_TEACH          0x5445  /* Teach                    */
#define PRODOS_AUX_T_GWP_DELUXEWRITE    0x8001  /* DeluxeWrite              */
#define PRODOS_AUX_T_GWP_APPLEWORKS_GS  0x8010  /* AppleWorks GS            */

/* GSS auxiliary types */
#define PRODOS_AUX_T_GSS_APPLEWORKS_GS  0x8010  /* AppleWorks GS            */

/* GDB auxiliary types */
#define PRODOS_AUX_T_GDB_APPLEWORKS_GS  0x8010  /* AppleWorks GS DB         */
#define PRODOS_AUX_T_GDB_AWGS_TMPL      0x8011  /* AWGS template            */
#define PRODOS_AUX_T_GDB_GSAS           0x8013

/* DRW auxiliary types */
#define PRODOS_AUX_T_DRW_OO_GRAPHICS    0x8013  /* AWGS O-O graphics        */

/* GDP auxiliary types */
#define PRODOS_AUX_T_GDP_GRAPHICWRITER  0x8002  /* A2gs GraphicWriter       */
#define PRODOS_AUX_T_GDP_APPLEWORKS_GS  0x8010  /* A2gs AWGS                */

/* HMD auxiliary types */
#define PRODOS_AUX_T_HMD_HYPERCARD_GS   0x0001  /* HyperCard GS             */
#define PRODOS_AUX_T_HMD_TUTOR_TECH     0x8001  /* Tutor-Tech               */
#define PRODOS_AUX_T_HMD_HYPERSTUDIO    0x8002  /* HyperStudio              */
#define PRODOS_AUX_T_HMD_NEXUS          0x8003  /* Nexus                    */

/* COM auxiliary types */
#define PRODOS_AUX_T_COM_APPLEWORKS_GS  0x8003  /* AppleWorks GS            */

/* MLR auxiliary types */
#define PRODOS_AUX_T_MLR_SCRIPT         0x005C  /* Mika City script         */
#define PRODOS_AUX_T_MLR_COLOR_TABLE    0xC7AB  /* Mika City color table    */
#define PRODOS_AUX_T_MLR_CHARACTER_DEF  0xCDEF  /* Mika City character def  */

/* LDF auxiliary types */
#define PRODOS_AUX_T_LDF_NIFTY_LIST_MOD 0x4001  /* Nifty list module        */
#define PRODOS_AUX_T_LDF_SUPER_INFO_MOD 0x4002  /* Super info module        */
#define PRODOS_AUX_T_LDF_TWILIGHT_MOD   0x4004  /* Twilight module          */
#define PRODOS_AUX_T_LDF_MARINETTI_LLM  0x4004  /* Marinetti link layer mod */

/* PNT auxiliary types */
#define PRODOS_AUX_T_PNT_PK_SUPER_HIRES 0x0001  /* Packed super hi-res      */
#define PRODOS_AUX_T_PNT_APPLE_PREF     0x0002  /* Apple preferred format   */
#define PRODOS_AUX_T_PNT_PK_QUICKDRAWII 0x0003  /* Packed QuickDraw II      */

/* PIC auxiliary types */
#define PRODOS_AUX_T_PIC_QUICKDRAW      0x0001  /* QuickDraw image          */
#define PRODOS_AUX_T_PIC_SHIRES_3200    0x0002  /* Super hi-res 3200        */

/* FON auxiliary types */
#define PRODOS_AUX_T_FON_QUICKDRAW_BIT  0x0000  /* QuickDraw bitmap font    */
#define PRODOS_AUX_T_FON_POINTLESS_TT   0x0001  /* Pointless TrueType font  */

/* SND auxiliary types */
#define PRODOS_AUX_T_SND_AIFF           0x0000  /* AIFF                     */
#define PRODOS_AUX_T_SND_AIFF_C         0x0001  /* AIFF-C                   */
#define PRODOS_AUX_T_SND_ASIF_INSTR     0x0002  /* ASIF instrument          */
#define PRODOS_AUX_T_SND_SOUND_RSRC     0x0003  /* Sound resource           */
#define PRODOS_AUX_T_SND_MIDI_SYNTH_WAV 0x0004  /* MIDI synth wave          */
#define PRODOS_AUX_T_SND_HYPERSTUDIO    0x8001  /* HyperStudio sound        */

/* LBR auxiliary types */
#define PRODOS_AUX_T_LBR_ALU            0x0000  /* ALU                      */
#define PRODOS_AUX_T_LBR_APPLE_SINGLE   0x0001  /* AppleSingle              */
#define PRODOS_AUX_T_LBR_APPLEDBL_HDR   0x0002  /* AppleDouble header       */
#define PRODOS_AUX_T_LBR_APPLEDBL_DATA  0x0003  /* AppleDouble data         */
#define PRODOS_AUX_T_LBR_BINARY_II      0x8000  /* Binary II                */
#define PRODOS_AUX_T_LBR_APPLELINK_ACU  0x8001  /* AppleLink ACU            */
#define PRODOS_AUX_T_LBR_SHRINKIT       0x8002  /* ShrinkIt                 */

/* LBR auxiliary types */
#define PRODOS_AUX_T_ATK_EASYMNT_ALIAS  0x0000  /* EasyMount alias          */

/* BAS auxiliary types */
#define PRODOS_AUX_T_BAS_PGM_LOAD_ADDR  0x0801  /* Applesoft pgm load addr  */



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The file stream implementation and the POSIX I/O functions will use the
** following variables to determine the file type and auxiliary type to use.
** This applies specifically to the fopen and open functions.
*/
extern unsigned char _filetype;  /* Default: PRODOS_T_BIN */
extern unsigned int _auxtype;    /* Default: 0            */

/* End of apple2_filetype.h */
#endif
