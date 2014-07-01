/*
** Compresses data to the DEFLATE format.
** The compressed data is ready to use with inflatemem().
** Compile using e.g.
** gcc -O2 -o deflater deflater.c -lz
**
** Author: Piotr Fusik <fox@scene.pl>
*/
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#define IN_SIZE_MAX 60000U
#define OUT_SIZE_MAX 60000U

int main(int argc, char* argv[])
{
        FILE* fp;
        char* inbuf;
        char* outbuf;
        size_t inlen;
        size_t outlen;
        z_stream stream;

        /* check command line */
        if (argc != 3) {
                fprintf(stderr,
                        "Compresses a file to the DEFLATE format.\n"
                        "24 Aug 2002, Piotr Fusik <fox@scene.pl>\n"
                        "Usage: deflater input_file deflated_file\n"
                );
                return 3;
        }

        /* alloc buffers */
        inbuf = malloc(IN_SIZE_MAX);
        outbuf = malloc(OUT_SIZE_MAX);
        if (inbuf == NULL || outbuf == NULL) {
                fprintf(stderr, "deflater: Out of memory!\n");
                return 1;
        }

        /* read input file */
        fp = fopen(argv[1], "rb");
        if (fp == NULL) {
                perror(argv[1]);
                return 1;
        }
        inlen = fread(inbuf, 1, IN_SIZE_MAX, fp);
        fclose(fp);

        /* compress */
        stream.next_in = inbuf;
        stream.avail_in = inlen;
        stream.next_out = outbuf;
        stream.avail_out = OUT_SIZE_MAX;
        stream.zalloc = (alloc_func) 0;
        stream.zfree = (free_func) 0;
        if (deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED,
                -MAX_WBITS, 9, Z_DEFAULT_STRATEGY) != Z_OK) {
                fprintf(stderr, "deflater: deflateInit2 failed\n");
                return 1;
        }
        if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
                fprintf(stderr, "deflater: deflate failed\n");
                return 1;
        }
        if (deflateEnd(&stream) != Z_OK) {
                fprintf(stderr, "deflater: deflateEnd failed\n");
                return 1;
        }

        /* write output */
        fp = fopen(argv[2], "wb");
        if (fp == NULL) {
                perror(argv[2]);
                return 1;
        }
        outlen = fwrite(outbuf, 1, stream.total_out, fp);
        fclose(fp);
        if (outlen != stream.total_out) {
                perror(argv[2]);
                return 1;
        }

        /* display summary */
        printf("Compressed %s (%d bytes) to %s (%d bytes)\n",
                argv[1], inlen, argv[2], outlen);
        return 0;
}
