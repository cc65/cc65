/*
** 2020-10-15, Greg King
**
** unsigned int __fastcall__ cbm_load (const char* name,
**                                     unsigned char device,
**                                     void* data);
*/

#include <cbm.h>
#include <limits.h>

/* Loads file "name" from the given device to the given address, or to the load
** address of the file if "data" is the null pointer (like load"name",8,1
** in BASIC).
** Returns the number of bytes that were loaded if loading was successful;
** otherwise 0; "_oserror" contains an error-code, then.
*/
unsigned int __fastcall__ cbm_load (const char* name, unsigned char device, void* data)
{
    void* load;
    int length;
    unsigned int size = 0;

    if (cbm_open (1, device, CBM_READ, name) != 0) {
        /* Can't load from a file that can't be openned. */
        return 0;
    }

    /* Get the file's load address. */
    if (cbm_read (1, &load, sizeof load) != sizeof load) {
        /* Either the file wasn't found, or it was too short.  (Note:
        ** the computer openned a file even if the drive couldn't open one.)
        */
        cbm_close (1);
        return 0;
    }

    /* If "data" doesn't hold an address, then use the file's address. */
    if (data == (void*)0x0000) {
        data = load;
    }

    /* Pull the file into RAM.  [Note that, if cbm_read() grabbed more
    ** than 32767 bytes at a time, then its result would look negative,
    ** which would cancel the load.]
    */
    do {
        size += (length = cbm_read (1, data, INT_MAX));
        data = (unsigned char*)data + length;
    } while (length == INT_MAX && cbm_k_readst() == 0);
    cbm_close (1);

    /* "length" is -1 if there was an error. */
    if (length < 0) {
        size = 0;
    }

    return size;
}
