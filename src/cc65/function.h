/*
 * function.h
 *
 * Ullrich von Bassewitz, 07.06.1998
 */



#ifndef FUNCTION_H
#define FUNCTION_H



/*****************************************************************************/
/*			    	     data				     */
/*****************************************************************************/



/* Structure that holds all data needed for function activation */
typedef struct Function Function;

/* Function activation data for current function (or NULL) */
extern Function* CurrentFunc;



/*****************************************************************************/
/*	      		    	     Code		     		     */
/*****************************************************************************/



const char* GetFuncName (const Function* F);
/* Return the name of the current function */

unsigned GetParamCount (const Function* F);
/* Return the parameter count for the current function */

unsigned GetParamSize (const Function* F);
/* Return the parameter size for the current function */

type* GetReturnType (Function* F);
/* Get the return type for the function */

int HasVoidReturn (const Function* F);
/* Return true if the function does not have a return value */

void RememberEntry (Function* F);
/* Remember the current output position for local space creation later */

unsigned GetRetLab (const Function* F);
/* Return the return jump label */

int ReserveLocalSpace (Function* F, unsigned Size);
/* Reserve (but don't allocate) the given local space and return the stack
 * offset.
 */

void AllocLocalSpace (Function* F);
/* Allocate any local space previously reserved. The function will do
 * nothing if there is no reserved local space.
 */

void NewFunc (struct SymEntry* Func);
/* Parse argument declarations and function body. */



/* End of function.h */
#endif



