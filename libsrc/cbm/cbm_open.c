/*
 * Marc 'BlackJack' Rintsch, 18.03.2001
 *
 * unsigned char cbm_open(unsigned char lfn, unsigned char device,
 *                        unsigned char sec_addr, const char* name);
 */
 
#include <cbm.h>

extern unsigned char _oserror;

/* It's like BASIC's: OPEN lfn, device, sec_addr, "name"
 */
unsigned char cbm_open(unsigned char lfn, unsigned char device,
                       unsigned char sec_addr, const char* name)
{
    cbm_k_setlfs(lfn, device, sec_addr);
    cbm_k_setnam(name);
    return _oserror = cbm_k_open();
}
