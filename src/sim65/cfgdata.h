/*****************************************************************************/
/*                                                                           */
/*				   cfgdata.h				     */
/*                                                                           */
/*	     		     Config data structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef CFGDATA_H
#define CFGDATA_H



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



typedef struct CfgData CfgData;
struct CfgData {
    enum {
        CfgDataInvalid,
	CfgDataId,
	CfgDataNumber,
	CfgDataString 
    }		Type;		/* Type of the value */
    union {
	char*	SVal;		/* String or id value */
	long	IVal;		/* Integer value */
    } V;
    unsigned    Line;           /* Line where the attribute was defined */
    unsigned    Col;            /* Column of attribute definition */
    char       	Attr[1];        /* The attribute name */
};



/* End of cfgdata.h */

#endif



