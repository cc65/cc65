/*
 * Marc 'BlackJack' Rintsch, 25.03.2001
 *
 * int cbm_write(unsigned char lfn, void* buffer, unsigned int size);
 */

#include <cbm.h>
#include <errno.h>

int cbm_write(unsigned char lfn, void* buffer, unsigned int size) {

    static unsigned int byteswritten;
    
    /* if we can't change to the outputchannel #lfn then return an error */
    if (_oserror = cbm_k_ckout(lfn)) return -1;
    
    byteswritten = 0;
    
    while (byteswritten<size && !cbm_k_readst()) {
        cbm_k_bsout(((unsigned char*)buffer)[byteswritten++]);
    }

    if (cbm_k_readst()) {
        _oserror = 5;       /* device not present */
        byteswritten = -1;
    }
        
    cbm_k_clrch();
    
    return byteswritten;
}
