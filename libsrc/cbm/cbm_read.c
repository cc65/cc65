
#include <cbm.h>

extern unsigned char _oserror;

int cbm_read(unsigned char lfn, void* buffer, unsigned int size)
{
    static unsigned int bytesread;
    static unsigned char tmp;
        
    if (_oserror = cbm_k_chkin(lfn)) return -1;
    
    bytesread = 0;
    
    while (bytesread<size) {
        tmp = cbm_k_basin();
        if (cbm_k_readst()) break;
        ((unsigned char*)buffer)[bytesread++] = tmp;
    }
    
    cbm_k_clrch();
    
    return bytesread;
}
