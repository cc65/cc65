/*****************************************************************************/
/*                                                                           */
/*				  funcdesc.h				     */
/*                                                                           */
/*	     Function descriptor structure for the cc65 C compiler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef FUNCDESC_H
#define FUNCDESC_H



/*****************************************************************************/
/*				struct FuncDesc				     */
/*****************************************************************************/



/* Masks for the Flags field in FuncDesc */
#define FD_IMPLICIT		0x0001U	/* Implicitly declared function      */
#define FD_EMPTY      		0x0002U	/* Function with empty param list    */
#define FD_VOID_PARAM   	0x0004U	/* Function with a void param list   */
#define FD_VARIADIC		0x0008U	/* Function with variable param list */
#define FD_FASTCALL		0x0010U	/* __fastcall__ function 	     */
#define FD_OLDSTYLE		0x0020U	/* Old style (K&R) function	     */
#define FD_UNNAMED_PARAMS	0x0040U	/* Function has unnamed params	     */

/* Bits that must be ignored when comparing funcs */
#define FD_IGNORE 	(FD_IMPLICIT | FD_UNNAMED_PARAMS)



/* Function descriptor */
typedef struct FuncDesc FuncDesc;
struct FuncDesc {
    unsigned		Flags;		/* Bitmapped flags FD_... 	     */
    struct SymTable*	SymTab;	  	/* Symbol table 		     */
    struct SymTable*   	TagTab;		/* Symbol table for structs/enums    */
    unsigned		ParamCount;	/* Number of parameters		     */
    unsigned		ParamSize;	/* Size of the parameters	     */
};



/*****************************************************************************/
/*	       	  	       	     Code				     */
/*****************************************************************************/



FuncDesc* NewFuncDesc (void);
/* Create a new symbol table with the given name */

void FreeFuncDesc (FuncDesc* E);
/* Free a function descriptor */



/* End of funcdesc.h */
#endif



