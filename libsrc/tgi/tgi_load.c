/*****************************************************************************/
/*                                                                           */
/*                                tgi_load.c                                 */
/*                                                                           */
/*                       Loader module for TGI drivers                       */
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



#include <string.h>
#include <fcntl.h>
#include <modload.h>
#include <tgi.h>
#include <tgi/tgi-kernel.h>



void __fastcall__ tgi_load (unsigned char mode)
/* Install the matching driver for the given mode. Will just load the driver
 * and check if loading was successul. Will not switch to gaphics mode.
 */
{
    const char* name = tgi_map_mode (mode);
    if (name == 0) {
        /* No driver for this mode */
        tgi_error = TGI_ERR_NO_DRIVER;
    } else {
        /* Load the driver */
        tgi_load_driver (name);
    }
}



void __fastcall__ tgi_load_driver (const char* name)
/* Install the given driver. This function is identical to tgi_load with the
 * only difference that the name of the driver is specified explicitly. You
 * should NOT use this function in most cases, use tgi_load() instead.
 */
{
    static struct mod_ctrl ctrl = {
        read            /* Read from disk */
    };
    unsigned Res;

    /* Check if we do already have a driver loaded. If so, remove it. */
    if (tgi_drv != 0) {
        tgi_unload ();
    }

    /* Now open the file */
    ctrl.callerdata = open (name, O_RDONLY);
    if (ctrl.callerdata >= 0) {

        /* Load the module */
        Res = mod_load (&ctrl);

        /* Close the input file */
        close (ctrl.callerdata);

        /* Check the return code */
        if (Res == MLOAD_OK) {

            /* Check the driver signature, install the driver */
            tgi_install (ctrl.module);
            return;

        }
    }

    /* Error loading the driver */
    tgi_error = TGI_ERR_CANNOT_LOAD;
}
