
#include <stdlib.h>
#include <stdio.h>

unsigned char buffer[512 * 1024];
unsigned len;
FILE *in, *out;
int n, i;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(-1);
    }
    in = fopen(argv[1], "rb");
    out = fopen(argv[2], "wb");
    if (!in || !out) {
        exit(-1);
    }
    len = fread(buffer, 1, 512 * 1024, in);
    n = 0; for (i = 0x1000; i < 0x2000; i++) {
        n += buffer[i];
    }
    buffer[0] = n & 0xff;
    buffer[1] = (n >> 8) & 0xff;
    fwrite(buffer, 1, len, out);
    fclose(in);
    fclose(out);
    return (0);
}
