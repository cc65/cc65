/*****************************************************************************/
/*                                                                           */
/*                                  chipdata.h                               */
/*                                                                           */
/*                        Chip description data structure                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef CHIPDATA_H
#define CHIPDATA_H



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Version information. */
#define CHIPDATA_VER_MAJOR      1U
#define CHIPDATA_VER_MINOR      0U

/* Forwards */
struct SimData;

/* ChipDesc structure */
typedef struct ChipData ChipData;
struct ChipData {
    const char* ChipName;       /* Name of the chip */
    unsigned    MajorVersion;   /* Version information */
    unsigned    MinorVersion;

    /* -- Exported functions -- */
    int           (*InitChip) (const struct SimData* Data);
    void*         (*InitInstance) (unsigned Addr, unsigned Range);
    void          (*WriteCtrl) (void* Data, unsigned Offs, unsigned char Val);
    void          (*Write) (void* Data, unsigned Offs, unsigned char Val);
    unsigned char (*ReadCtrl) (void* Data, unsigned Offs);
    unsigned char (*Read) (void* Data, unsigned Offs);
};



/* End of chipdata.h */

#endif



