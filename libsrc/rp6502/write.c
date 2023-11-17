#include <rp6502.h>
#include <unistd.h>

int __fastcall__ write (int fildes, const void* buf, unsigned count)
{
    int ax, total = 0;
    while (count) {
        int blockcount = (count > 256) ? 256 : count;
        ax = write_xstack (&((char*)buf)[total], blockcount, fildes);
        if (ax < 0) {
            return ax;
        }
        total += ax;
        count -= ax;
        if (ax < blockcount) {
            break;
        }
    }
    return total;
}
