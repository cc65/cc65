/*
 * Marc 'BlackJack' Rintsch, 06.03.2001
 *
 * unsigned char cbm_load(const char* name, char device, unsigned int addr);
 */

#include <cbm.h>

/* loads file "name" from given device to given address or to the load address
 * of the file if addr is 0
 */
unsigned char cbm_load(const char* name, unsigned char device,
                       unsigned int addr)
{
    /* LFN is set to 0 but it's not needed for loading.
     * (BASIC V2 sets it to the value of the SA for LOAD)
     */
    cbm_k_setlfs(0, device, addr == 0);
    cbm_k_setnam(name);
    return cbm_k_load(0, addr);
}
