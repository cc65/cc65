/* This is a very simplified version of the POSIX opendir(),  */
/* readdir(), and closedir() -- for Commodore computers.      */
/* Created by Josef Soucek, 2003.  E-mail: josef.soucek@ct.cz */

/* 2003-01-21 -- Version 0.1 */
/* 2009-10-10 -- Version 0.3 */
/* 2011-04-07 -- Version 0.4, groepaz */
/* 2011-04-14 -- Version 0.5, Greg King */

/* Tested with floppy-drive and IDE64 devices.        */
/* Not tested with messed (buggy) directory listings. */
/* Limits filenames to 16 chars. (VICE supports more  */
/* in directory listings).                            */


#include <stdarg.h>
#include <cbm.h>
#include <errno.h>



/* Opens directory listing. Returns 0 if opening directory was successful;
** otherwise, an error-code corresponding to cbm_open(). As an optional
** argument, the name of the directory may be passed to the function. If
** no explicit name is specified, "$" is used.
*/
unsigned char cbm_opendir (unsigned char lfn, unsigned char device, ...)
{
    va_list ap;
    const char* name = "$";

    /* The name used in cbm_open may optionally be passed */
    if (__argsize__ == 4) {
        va_start (ap, device);
        name = va_arg (ap, const char*);
        va_end (ap);
    }

    /* Open the directory */
    if (cbm_open (lfn, device, CBM_READ, name) == 0) {
        if ((_oserror = cbm_k_chkin (lfn)) == 0) {
            /* Ignore start address */
            cbm_k_basin();
            cbm_k_basin();
            cbm_k_clrch();
            if (cbm_k_readst()) {
                cbm_close(lfn);
                _oserror = 4;           /* directory cannot be read */
            }
        }
    }
    return _oserror;
}



/* Reads one directory line into cbm_dirent structure.
** Returns 0 if reading directory-line was successful.
** Returns non-zero if reading directory failed, or no more file-names to read.
** Returns 2 on last line.  Then, l_dirent->size = the number of "blocks free."
*/
unsigned char __fastcall__ cbm_readdir (unsigned char lfn, register struct cbm_dirent* l_dirent)
{
    unsigned char byte, i = 0;
    unsigned char is_header = 0;
    unsigned char rv = 1;

    if (!cbm_k_chkin(lfn)) {
        if (!cbm_k_readst()) {
            /* skip 2 bytes, next-BASIC-line pointer */
            cbm_k_basin();
            cbm_k_basin();

            /* File-size or drive/partition number */
            l_dirent->size = cbm_k_basin() | (cbm_k_basin() << 8);

            byte = cbm_k_basin();
            switch (byte) {

                /* "B" BLOCKS FREE. */
              case 'b':
                /* Read until end; careless callers might call us again. */
                while (!cbm_k_readst()) {
                    cbm_k_basin();
                }
                rv = 2; /* EOF */
                goto ret_val;

                /* Reverse-text shows when this is the directory header. */
              case 0x12:  /* RVS_ON */
                is_header = 1;
            }

            while (byte != '\"') {
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 3;
                    goto ret_val;
                }
                byte = cbm_k_basin();
            }

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

            if (is_header) {
                l_dirent->type = CBM_T_HEADER;

                /* Get the disk-format code. */
                i = 6;
                do {
                    l_dirent->access = byte = cbm_k_basin();
                } while (--i != 0);

            } else {
                /* Go to the file-type column. */
                while ((byte = cbm_k_basin()) == ' ') {
                    /* prevent endless loop */
                    if (cbm_k_readst()) {
                        rv = 5;
                        goto ret_val;
                    }
                }

                l_dirent->access = CBM_A_RW;

                /* "Splat" files shouldn't be read. */
                if (byte == '*') {
                    l_dirent->access = CBM_A_WO;
                    byte = cbm_k_basin();
                }

                /* Determine the file type */
                l_dirent->type = _cbm_filetype (byte);

                /* Notice whether it's a directory or a deleted file. */
                if (cbm_k_basin() == 'i' && byte == 'd') {
                    l_dirent->type = CBM_T_DIR;
                }
                cbm_k_basin();

                /* Locked files shouldn't be written. */
                if ((byte = cbm_k_basin()) == '<') {
                    l_dirent->access = (l_dirent->access == CBM_A_WO)
                        ? 0 : CBM_A_RO;
                }
            }

            /* Read to the end of the line. */
            while (byte != 0) {
                /* prevent endless loop */
                if (cbm_k_readst()) {
                    rv = 6;
                    goto ret_val;
                }
                byte = cbm_k_basin();
            }

            rv = 0;
            goto ret_val;
        }
    }

ret_val:
    cbm_k_clrch();
    return rv;
}



void __fastcall__ cbm_closedir (unsigned char lfn)
{
    cbm_close(lfn);
}


