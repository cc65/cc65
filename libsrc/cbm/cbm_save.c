/*
** Marc 'BlackJack' Rintsch, 11.03.2001
**
** unsigned char cbm_save(const char* name,
**                        char device,
**                        unsigned char* data,
**                        unsigned int size);
*/

#include <cbm.h>
#include <errno.h>

/* saves a memory area from start to end-1 to a file.
*/
unsigned char __fastcall__ cbm_save (const char* name, 
                                     unsigned char device,
                                     const void* data, 
                                     unsigned int size)
{
    cbm_k_setlfs(0, device, 0);
    cbm_k_setnam(name);
    return _oserror =
            cbm_k_save((unsigned int)data, ((unsigned int)data) + size);
}
