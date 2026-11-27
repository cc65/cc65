#include "o65check.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#define PRGNAME "o65check"

// #define VERBOSE 1

#if VERBOSE
  #define VERBOSE_FPRINTF(...) \
      fprintf(__VA_ARGS__)
#else
  #define VERBOSE_FPRINTF(...) do { } while (0)
#endif

struct file_s {
    size_t  length;
    size_t  readpointer;
    uint8_t data[];
};

void * my_alloc(size_t member, size_t size)
{
    void * p = calloc(member, size);
    if (!p) {
        fprintf(stderr, "Out of memory error.\n");
        exit(5);
    }

    return p;
}

file_t * file_open(const char * const filename)
{
    file_t * file = NULL;

    FILE * f = fopen(filename, "rb");

    if (!f) {
        fprintf(stderr, PRGNAME ": Error opening file %s.\n", filename);
        goto error;
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);

    rewind(f);

    file = my_alloc(1, sizeof *file + length);

    if (fread(file->data, length, 1, f) != 1) {
        fprintf(stderr, PRGNAME ": Error reading file %s.\n", filename);
        goto error;
    }

    file->length = length;

    fclose(f);

    return file;

error:
    free(file);
    fclose(f);

    return NULL;
}

size_t file_read_n_byte(file_t * file, uint8_t * data, size_t count)
{
    assert(file->readpointer < file->length);

    if (file->readpointer + count <= file->length) {
        memcpy(data, &file->data[file->readpointer], count);
        file->readpointer += count;

        return count;
    }
    else {
        // EOF
        return -1;
    }
}

void * file_get_pointer_to_current(file_t * file)
{
    return &file->data[file->readpointer];
}

void file_close(file_t * file)
{
    free(file);
}
