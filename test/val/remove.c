#include <stdio.h>
#include <string.h>
#include <unistd.h>

int fails = 0;


static void create_out_file(const char *outfile_path) {
    FILE *out;


    out = fopen(outfile_path, "wb");
    if (out == NULL) {
        printf("Could not create %s\n", outfile_path);
        fails++;
        return;
    }
    fclose(out);
}

int main (int argc, char **argv)
{
    int r;
    static char outfile_path[FILENAME_MAX+1];

    sprintf(outfile_path, "%s.test.out", argv[0]);

    create_out_file(outfile_path);
    r = remove(outfile_path);
    if (r != 0) {
        printf("could not remove() %s\n", outfile_path);
        fails++;
    }

    create_out_file(outfile_path);
    r = unlink(outfile_path);
    if (r != 0) {
        printf("could not unlink() %s\n", outfile_path);
        fails++;
    }

    r = remove("klsdfjqlsjdflkqjdsoizu");
    if (r == 0) {
        printf("remove()ing non-existent file succeeded\n");
        fails++;
    }

    r = unlink("klsdfjqlsjdflkqjdsoizu");
    if (r == 0) {
        printf("unlink()ing non-existent file succeeded\n");
        fails++;
    }

    return fails;
}
