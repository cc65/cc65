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



/* common */
#include "coll.h"



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



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



CfgData* NewCfgData (void);
/* Create and intialize a new CfgData struct, then return it. The function
 * uses the current output of the config scanner.
 */

void FreeCfgData (CfgData* D);
/* Free a config data structure */

int CfgDataFind (Collection* Attributes, const char* AttrName);
/* Find the attribute with the given name and return its index. Return -1 if
 * the attribute was not found.
 */

int CfgDataGetId (Collection* Attributes, const char* Name, char** Id);
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, copy it into Buf and return
 * true. If not found, return false.
 */

int CfgDataGetStr (Collection* Attributes, const char* Name, char** S);
/* Search CfgInfo for an attribute with the given name and type "string".
 * If found, remove it from the configuration, copy it into Buf and return
 * true. If not found, return false.
 */

int CfgDataGetNum (Collection* Attributes, const char* Name, long* Val);
/* Search CfgInfo for an attribute with the given name and type "number".
 * If found, remove it from the configuration, copy it into Val and return
 * true. If not found, return false.
 */



/* End of cfgdata.h */

#endif



