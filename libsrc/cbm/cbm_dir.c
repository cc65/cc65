/* This is very simplified version of POSIX opendir(), readdir() and closedir() */
/* for Commodore computers.                                                     */
/* Created by Josef Soucek, 2003    E-mail:josef.soucek@ct.cz                   */

/* Version 0.1 - 21.1.2003 */
/* Tested with floppy drive and IDE64 devices                                   */
/* Not tested with messed (buggy) directory listing                             */
/* Limits filenames to 16 chars (VICE supports more in directory listing)       */



#include <cbm.h>



unsigned char __fastcall__ cbm_opendir (unsigned char lfn, unsigned char device)
{
    unsigned char status;
    if ((status = cbm_open (lfn, device, CBM_READ, "$")) == 0) {
        if (cbm_k_chkin (lfn) == 0) {
            if (cbm_k_basin () == 0x01) {         /* Start address */
                if (cbm_k_basin () == 0x04) {
                    cbm_k_clrch ();
                    return 0;
                }
            } else {
                cbm_close (lfn);
                cbm_k_clrch ();
                return 2;
            }
        }
    }
    return status;
}



unsigned char __fastcall__ cbm_readdir (unsigned char lfn, register struct cbm_dirent* l_dirent)
{
    unsigned char byte, i;

    if (cbm_k_chkin (lfn) == 0) {
        if (cbm_k_readst () == 0) {
            cbm_k_basin ();                    /* 0x01, 0x01, next basic line */
            cbm_k_basin ();

            byte = cbm_k_basin();             /* File-size */
            l_dirent->size = byte | ((cbm_k_basin()) << 8);

            byte = cbm_k_basin();

            if (byte == 'b') {                 /* "B" BLOCKS FREE. */
                while (cbm_k_readst () == 0) { /* Read until end */
                    cbm_k_basin ();
                }
                cbm_k_clrch ();
                return 2;                       /* END */
            }

            if (byte != '\"') {
                while (cbm_k_basin() != '\"') {
                    if (cbm_k_readst () != 0) {   /* ### Included to prevent */
                        cbm_k_clrch ();           /* ### Endless loop */
                        return 3;                 /* ### Should be probably removed */
                    }                             /* ### */
                }
            }

            i = 0;
            while ((byte = cbm_k_basin ()) != '\"') {
                if (cbm_k_readst () != 0) {     /* ### Included to prevent */
                    cbm_k_clrch ();             /* ### Endless loop */
                    return 4;                   /* ### Should be probably removed */
                }                               /* ### */

                if (i < sizeof (l_dirent->name) - 1) {
                    l_dirent->name[i] = byte;
                    ++i;
                }
            }
            l_dirent->name[i] = '\0';

            while ((byte=cbm_k_basin ()) == ' ') {
                if (cbm_k_readst ()) {          /* ### Included to prevent */
                    cbm_k_clrch ();             /* ### Endless loop */
                    return 5;                   /* ### Should be probably removed */
                }                               /* ### */
            }

            switch (byte) {
                case 's':
                    l_dirent->type = CBM_T_SEQ;
                    break;
                case 'p':
                    l_dirent->type = CBM_T_PRG;
                    break;
                case 'u':
                    l_dirent->type = CBM_T_USR;
                    break;
                case 'r':
                    l_dirent->type = CBM_T_REL;
                    break;
                case 'c':
                    l_dirent->type = CBM_T_CBM;
                    break;
                case 'd':
                    l_dirent->type = CBM_T_DIR;
                    break;
                case 'v':
                    l_dirent->type = CBM_T_VRP;
                    break;
                default:
                    l_dirent->type = CBM_T_OTHER;
            }

            cbm_k_basin ();
            cbm_k_basin ();

            byte = cbm_k_basin ();

            l_dirent->access = (byte == 0x3C)? CBM_A_RO : CBM_A_RW;

            if (byte != 0) {
                while (cbm_k_basin() != 0) {
                    if (cbm_k_readst () != 0) { /* ### Included to prevent */
                        cbm_k_clrch ();         /* ### Endless loop */
                        return 6;               /* ### Should be probably removed */
                    }                           /* ### */
                }
            }

            cbm_k_clrch ();
            return 0;                         /* Line read successfuly */
        }
    }
    cbm_k_clrch ();
    return 1;
}


void __fastcall__ cbm_closedir( unsigned char lfn)
{
    cbm_close(lfn);
}


