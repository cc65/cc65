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
            /* Ignore start address */
            cbm_k_basin();
            cbm_k_basin();
            if (cbm_k_readst()) {
                cbm_close(lfn);
                status = 2;
                cbm_k_clrch();
            } else {
                status = 0;
                cbm_k_clrch();
            }
        }
    }
    return status;
}



unsigned char __fastcall__ cbm_readdir (unsigned char lfn, register struct cbm_dirent* l_dirent)
{
    unsigned char byte, i;
    unsigned char rv;
    unsigned char is_header;
    static const unsigned char types[] = {
        CBM_T_OTHER, CBM_T_OTHER, CBM_T_CBM,   CBM_T_DEL,   /* a b c d */
        CBM_T_OTHER, CBM_T_OTHER, CBM_T_OTHER, CBM_T_OTHER, /* e f g h */
        CBM_T_OTHER, CBM_T_OTHER, CBM_T_OTHER, CBM_T_OTHER, /* i j k l */
        CBM_T_OTHER, CBM_T_OTHER, CBM_T_OTHER, CBM_T_PRG,   /* m n o p */
        CBM_T_OTHER, CBM_T_REL,   CBM_T_SEQ,   CBM_T_OTHER, /* q r s t */
        CBM_T_USR,   CBM_T_VRP                              /* u v     */
    };

    rv = 1;
    is_header = 0;

    if (!cbm_k_chkin(lfn)) {
        if (!cbm_k_readst()) {
            /* skip 2 bytes, next basic line pointer */
            cbm_k_basin();
            cbm_k_basin();

            /* File-size */
            l_dirent->size = cbm_k_basin() | ((cbm_k_basin()) << 8);

            byte = cbm_k_basin();

            /* "B" BLOCKS FREE. */
            if (byte == 'b') {
                /* Read until end, careless callers may call us again */
                while (!cbm_k_readst()) {
                    cbm_k_basin();
                }
                rv = 2; /* EOF */
                goto ret_val;
            }

            /* reverse text shows that this is the directory header */
            if (byte == 0x12) { /* RVS_ON */
                is_header = 1;
            }

            while (byte != '\"') {
                byte = cbm_k_basin();
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 3;
                    goto ret_val;
                }
            }

            i = 0;
            while ((byte = cbm_k_basin()) != '\"') {
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 4;
                    goto ret_val;
                }

                if (i < sizeof (l_dirent->name) - 1) {
                    l_dirent->name[i] = byte;
                    ++i;
                }
            }
            l_dirent->name[i] = '\0';

            while ((byte = cbm_k_basin()) == ' ') {
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 5;
                    goto ret_val;
                }
            }

            if (is_header) {
                l_dirent->type = CBM_T_HEADER;
            } else {
                if (byte >= 'a' && byte < 'a' + sizeof(types)) {
                    l_dirent->type = types[byte - 'a'];
                } else {
                    l_dirent->type = CBM_T_OTHER;
                }

                if ((cbm_k_basin() == 'i') && (l_dirent->type == CBM_T_DEL)) {
                    l_dirent->type = CBM_T_DIR;
                }
                cbm_k_basin();

                byte = cbm_k_basin();

                l_dirent->access = (byte == 0x3C)? CBM_A_RO : CBM_A_RW;
            }

            /* read to end of line */
            while (byte != 0) {
                byte = cbm_k_basin();
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 6;
                    goto ret_val;
                }
            }

            rv = 0;
            goto ret_val;
        }
    }

ret_val:
    cbm_k_clrch();
    return rv;
}


void __fastcall__ cbm_closedir( unsigned char lfn)
{
    cbm_close(lfn);
}


