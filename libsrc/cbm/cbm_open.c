/*
 * Marc 'BlackJack' Rintsch, 18.03.2001
 *
 * unsigned char cbm_open(unsigned char lfn, unsigned char device,
 *                        unsigned char sec_addr, const char* name)
 */
 
#include <cbm.h>

unsigned char cbm_open(unsigned char lfn, unsigned char device,
                       unsigned char sec_addr, const char* name)
{
    cbm_k_setlfs(lfn, device, sec_addr);
    cbm_k_setnam(name);
    return cbm_k_open();
}
