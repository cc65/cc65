#include <rp6502.h>
#include <unistd.h>

int __fastcall__ read (int fildes, void* buf, unsigned count)
{
    int total = 0;
    while (count) {
        unsigned blockcount = (count > 256) ? 256 : count;
        int bytes_read = read_xstack (&((char*)buf)[total], blockcount, fildes);
        if (bytes_read < 0) {
            return bytes_read;
        }
        total += bytes_read;
        count -= bytes_read;
        if (bytes_read < blockcount) {
            break;
        }
    }
    return total;
}
