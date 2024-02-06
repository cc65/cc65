/*****************************************************************************/
/*                                                                           */
/*                           tgi_load_vectorfont.c                           */
/*                                                                           */
/*                  Loader module for TGI vector font files                  */
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



#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <tgi.h>
#include <tgi/tgi-kernel.h>
#include <tgi/tgi-vectorfont.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const tgi_vectorfont* __fastcall__ tgi_load_vectorfont (const char* name)
/* Load a vector font into memory and return it. In case of errors, NULL is
** returned and an error is set, which can be retrieved using tgi_geterror.
** To use the font, it has to be installed using tgi_install_vectorfont.
*/
{
    static const char Magic[4] = {
        0x54, 0x43, 0x48, TGI_VF_VERSION
    };

    tgi_vectorfont_header H;
    int                   F;
    tgi_vectorfont*       Font = 0;
    unsigned              V;
    unsigned char         I;


    /* Assume we have an error loading the font */
    tgi_error = TGI_ERR_CANNOT_LOAD;

    /* Open the file */
    F = open (name, O_RDONLY);
    if (F < 0) {
        /* Cannot open file */
        goto LoadError;
    }

    /* Read the header */
    if (read (F, &H, sizeof (H)) != sizeof (H)) {
        /* Cannot read header bytes */
        goto LoadError;
    }

    /* Check the header */
    if (memcmp (&H, Magic, sizeof (Magic)) != 0) {
        /* Header magic not ok */
        goto LoadError;
    }

    /* Allocate memory for the data */
    Font = malloc (H.size);
    if (Font == 0) {
        /* Out of memory */
        tgi_error = TGI_ERR_NO_RES;
        goto LoadError;
    }

    /* Read the whole font file into the buffer */
    if (read (F, Font, H.size) != H.size) {
        /* Problem reading font data */
        free (Font);
        goto LoadError;
    }

    /* Close the file */
    close (F);

    /* Fix the offset pointers. When loaded, they contain numeric offsets
    ** into the VectorOps, with the start of the VectorOps at offset zero.
    ** We will add a pointer to the VectorOps to make them actual pointers
    ** that may be used independently from anything else.
    */
    V = (unsigned) &Font->vec_ops;
    for (I = 0; I < TGI_VF_CCOUNT; ++I) {
        Font->chars[I] += V;
    }

    /* Clear the error */
    tgi_error = TGI_ERR_OK;

    /* Return the vector font loaded */
    return Font;

LoadError:
    /* Some sort of load problem. If the file is still open, be sure to
    ** close it
    */
    if (F >= 0) {
        close (F);
    }
    return 0;
}



