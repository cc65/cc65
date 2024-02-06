/*****************************************************************************/
/*                                                                           */
/*                             tgi-vectorfont.h                              */
/*                                                                           */
/*                        TGI vector font definitions                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2009,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef _TGI_VECTORFONT_H
#define _TGI_VECTORFONT_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define TGI_VF_FIRSTCHAR        0x20            /* First char in file */
#define TGI_VF_LASTCHAR         0x7E            /* Last char in file */
#define TGI_VF_CCOUNT           (TGI_VF_LASTCHAR - TGI_VF_FIRSTCHAR + 1)

#define TGI_VF_VERSION          0x00            /* File version number */

/* TCH file header */
typedef struct tgi_vectorfont_header tgi_vectorfont_header;
struct tgi_vectorfont_header {
    unsigned char       magic[3];               /* "TCH" */
    unsigned char       version;                /* Version number */
    unsigned            size;                   /* Font data size */
};

/* Font data loaded directly from file */
struct tgi_vectorfont {
    unsigned char       top;                    /* Height of char */
    unsigned char       baseline;               /* Character baseline */
    unsigned char       bottom;                 /* Descender */
    unsigned char       widths[TGI_VF_CCOUNT];  /* Char widths */
    unsigned char*      chars[TGI_VF_CCOUNT];   /* Pointer to character defs */
    unsigned char       vec_ops[1];             /* Actually dynamic */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void __fastcall__ tgi_vectorchar (char C);
/* Draw one character of the vector font at the current graphics cursor
** position using the current font magnification.
*/



/* End of tgi-vectorfont.h */
#endif



