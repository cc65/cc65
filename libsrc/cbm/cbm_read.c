/*
 * Marc 'BlackJack' Rintsch, 19.03.2001
 *
 * int cbm_read(unsigned char lfn, void* buffer, unsigned int size);
 */

#include <cbm.h>

extern unsigned char _oserror;

int cbm_read(unsigned char lfn, void* buffer, unsigned int size)
{
    static unsigned int bytesread;
    static unsigned char tmp;

    /* if we can't change to the inputchannel #lfn then return an error */
    if (_oserror = cbm_k_chkin(lfn)) return -1;
    
    bytesread = 0;

    while (bytesread<size && !cbm_k_readst()) {
        tmp = cbm_k_basin();
        
        /* the kernal routine BASIN sets ST to EOF if the end of file
         * is reached the first time, then we have store tmp.
         * every subsequent call returns EOF and READ ERROR in ST, then
         * we have to exit the loop here immidiatly. */
        if (cbm_k_readst() & 0xBF) break;
        
        ((unsigned char*)buffer)[bytesread++] = tmp;
    }
    
    cbm_k_clrch();
    return bytesread;
}
