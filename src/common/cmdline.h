/*****************************************************************************/
/*                                                                           */
/*				   cmdline.h				     */
/*                                                                           */
/*		   Helper functions for command line parsing		     */
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



#ifndef CMDLINE_H
#define CMDLINE_H



/*****************************************************************************/
/*     	       	       	       	     Data				     */
/*****************************************************************************/



/* Structure defining a long option */
typedef struct LongOpt	LongOpt;
struct LongOpt {
    const char*	Option;
    unsigned	ArgCount;
    void 	(*Func) (const char* Opt, const char* Arg);
};



/*****************************************************************************/
/*     	       	       	       	     Code				     */
/*****************************************************************************/



void InitCmdLine (unsigned aArgCount, char* aArgVec[]);
/* Initialize command line parsing. aArgVec is the argument array terminated by
 * a NULL pointer (as usual), ArgCount is the number of valid arguments in the
 * array. Both arguments are remembered in static storage.
 */

void UnknownOption (const char* Opt);
/* Print an error about an unknown option. */

void NeedArg (const char* Opt);
/* Print an error about a missing option argument and exit. */

void InvSym (const char* Def);
/* Print an error about an invalid symbol definition and die */

const char* GetArg (int* ArgNum, unsigned Len);
/* Get an argument for a short option. The argument may be appended to the
 * option itself or may be separate. Len is the length of the option string.
 */

void LongOption (int* ArgNum, const LongOpt* OptTab, unsigned OptCount);
/* Handle a long command line option */



/* End of cmdline.h */

#endif



