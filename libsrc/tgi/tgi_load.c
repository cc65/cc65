/*
 * TEST VERSION - CURRENTLY C64 SPECIFIC!!!
 */



#include <string.h>
#include <cbm.h>
#include <modload.h>
#include <tgi.h>
#include <tgi/tgi-kernel.h>



static unsigned char ReadInputBlock (struct mod_ctrl* C, void* Buf, unsigned Size)
{
    return (cbm_read (1, Buf, Size) != Size);
}



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
        /* Laod the driver */
        tgi_load_driver (name);
    }
}



void __fastcall__ tgi_load_driver (const char* name)
/* Install the given driver. This function is identical to tgi_load with the
 * only difference that the name of the driver is specified explicitly. You
 * should NOT use this function in most cases, use tgi_load() instead.
 */
{
    static const unsigned char marker[4] = { 0x74, 0x67, 0x69, 0x00 };

    static struct mod_ctrl ctrl = {
        ReadInputBlock           /* read_block */
    };
    unsigned Res;

    /* Check if we do already have a driver loaded. If so, remove it. */
    if (tgi_drv != 0) {
        tgi_unload ();
    }

    /* Now open the file */
    if (cbm_open (1, 8, 2, name) != 0) {
        tgi_error = TGI_ERR_CANNOT_LOAD;
        return;
    }

    /* Load the module */
    Res = mod_load (&ctrl);
    cbm_close (1);

    /* Check the return code */
    if (Res == MLOAD_OK) {

        /* Get a pointer to the loaded driver */
        tgi_drv = (tgi_drv_header*) ctrl.module;

        /* Check the header */
        if (memcmp (tgi_drv, marker, sizeof (marker)) != 0) {
            /* Invalid driver */
            mod_free (tgi_drv);
            tgi_drv = 0;
            tgi_error = TGI_ERR_INV_DRIVER;
        } else {
            /* Driver is ok do setup */
            tgi_setup ();
        }

    } else {

        /* Error loading the driver */
        tgi_error = TGI_ERR_CANNOT_LOAD;

    }
}
