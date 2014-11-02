/*****************************************************************************/
/*                                                                           */
/*                                  model.h                                  */
/*                                                                           */
/*         o65 model definitions for the co65 object file converter          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef MODEL_H
#define MODEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Supported models */
typedef enum {
    O65_MODEL_INVALID = -1,     /* Invalid model */
    O65_MODEL_NONE,             /* No model given */
    O65_MODEL_OSA65,            /* Not implemented */
    O65_MODEL_LUNIX,            /* Not implemented */
    O65_MODEL_CC65_MODULE,

    O65_MODEL_COUNT             /* Number of available models */
} O65Model;



/* Current model */
extern O65Model Model;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* GetModelName (O65Model M);
/* Map the model to its name. */

O65Model FindModel (const char* ModelName);
/* Map a model name to its identifier. Return O65_MODEL_INVALID if the name
** could not be found. Case is ignored when comparing names.
*/



/* End of model.h */

#endif
