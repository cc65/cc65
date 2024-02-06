/*****************************************************************************/
/*                                                                           */
/*                                 seqpoint.h                                */
/*                                                                           */
/*                     Stuff involved in sequence points                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* Copyright 2022 The cc65 Authors                                           */
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



#ifndef SEQPOINT_H
#define SEQPOINT_H



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



#define SQP_KEEP_NONE   0x00U
#define SQP_KEEP_A      0x01U
#define SQP_KEEP_AX     0x03U
#define SQP_KEEP_EAX    0x07U
#define SQP_MASK_EAX    0x07U
#define SQP_KEEP_TEST   0x10U
#define SQP_KEEP_EXPR   0x17U       /* SQP_KEEP_TEST | SQP_KEEP_EAX */



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



void SetSQPFlags (unsigned Flags);
/* Set the SQP_KEEP_* flags for the deferred operations in the statement */

unsigned GetSQPFlags (void);
/* Get the SQP_KEEP_* flags for the deferred operations in the statement */



/* End of seqpoint.h */

#endif
