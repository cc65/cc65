/*
 * stmt.h
 *
 * Ullrich von Bassewitz, 19.06.1998
 */



#ifndef STMT_H
#define STMT_H



/*****************************************************************************/
/*	    			     Code		     		     */
/*****************************************************************************/



int Statement (int* PendingToken);
/* Statement parser. Returns 1 if the statement does a return/break, returns
 * 0 otherwise. If the PendingToken pointer is not NULL, the function will
 * not skip the terminating token of the statement (closing brace or
 * semicolon), but store true if there is a pending token, and false if there
 * is none. The token is always checked, so there is no need for the caller to
 * check this token, it must be skipped, however. If the argument pointer is
 * NULL, the function will skip the token.
 */



/* End of stmt.h */

#endif



