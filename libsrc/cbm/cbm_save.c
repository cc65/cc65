/*
 * Marc 'BlackJack' Rintsch, 11.03.2001
 *
 * unsigned char cbm_save(const char* name,
 *                        char device,
 *                        unsigned int start,
 *                        unsigned int end);
 */

#include <cbm.h>

/* saves a memory area from start to end-1 to a file.
 */
unsigned char cbm_save(const char* name, unsigned char device,
                       unsigned int start, unsigned int end)
{
    cbm_k_setlfs(0, device, 0);
    cbm_k_setnam(name);
    return cbm_k_save(start, end);
}
