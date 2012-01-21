#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <dio.h>

dhandle_t dhandle;

struct dir_entry_t {
    struct {
        unsigned name_length  :4;
        unsigned storage_type :4;
    }             storage_length;
    char          file_name[15];
    unsigned char file_type;
    unsigned      key_pointer;
    unsigned      blocks_used;
    unsigned char size[3];
    unsigned long creation;
    unsigned char version;
    unsigned char min_version;
    unsigned char access;
    unsigned      aux_type;
    unsigned long last_mod;
    unsigned      header_pointer;
}* dir_entry;

union {
    unsigned char bytes[512];
    struct {
        unsigned      prev_block;
        unsigned      next_block;
        unsigned char entries[1];
    } content;
} dir_block;

union {
    unsigned char bytes[512];
    struct {
        unsigned char addr_lo[254];
        unsigned char size_lo[2];
        unsigned char addr_hi[254];
        unsigned char size_hi[2];
    } content;
} index_block;

union {
    unsigned char bytes[512];
    struct {
        unsigned char      info_block[256];
        unsigned char      vlir_records[128];
        struct dir_entry_t dir_entry;
    } content;
} header_block;


static void err_exit(char *operation)
{
    fprintf(stderr, "%s - err:%02x - %s",
            operation, (int)_oserror, _stroserror(_oserror));
    getchar();
    exit(EXIT_FAILURE);
}


static unsigned get_dir_entry(char* p_name)
{
    char*          d_name;
    char*          f_name;
    size_t         f_namelen;
    DIR*           dir;
    struct dirent* dirent;
    unsigned       cur_addr;
    unsigned char  entry_length;
    unsigned char  entries_per_block;
    unsigned char  cur_entry;

    /* Split path name into directory name and file name */
    f_name = strrchr(p_name, '/');
    if (f_name) {
        d_name = p_name;
        *f_name++ = '\0';
    } else {
        d_name = ".";
        f_name = p_name;
    }
    f_namelen = strlen(f_name);

    /* Start with high level functions to get handling
       of relative path and current drive for free */
    dir = opendir(d_name);
    if (!dir) {
        err_exit("opendir");
    }
    dirent = readdir(dir);
    if (!dirent) {
        err_exit("readdir");
    }

    /* Field header_pointer directly follows field last_mod */
    cur_addr = *(unsigned*)(&dirent->d_mtime.hour + 1);

    /* DEV_NUM is set to the drive accessed above */
    dhandle = dio_open(*(driveid_t*)0xBF30);
    if (!dhandle) {
        err_exit("dio_open");
    }

    if (dio_read(dhandle, cur_addr, &dir_block)) {
        err_exit("dio_read.1");
    }

    /* Get directory entry infos from directory header */
    entry_length      = dir_block.bytes[0x23];
    entries_per_block = dir_block.bytes[0x24];

    /* Skip directory header entry */
    cur_entry = 1;

    do {

        /* Search for next active directory entry */
        do {

            /* Check if next directory block is necessary */
            if (cur_entry == entries_per_block) {

                /* Check if another directory block is present */
                cur_addr = dir_block.content.next_block;
                if (!cur_addr) {
                    _mappederrno(0x46);
                    err_exit("dio_read.2");
                }

                /* Read next directory block */
                if (dio_read(dhandle, cur_addr, &dir_block)) {
                    err_exit("dio_read.3");
                }

                /* Start with first entry in next block */
                cur_entry = 0;
            }

            /* Compute pointer to current entry */
            dir_entry = (struct dir_entry_t*)(dir_block.content.entries +
                                              cur_entry * entry_length);

            /* Switch to next entry */
            ++cur_entry;
        } while (!dir_entry->storage_length.storage_type);

    } while (dir_entry->storage_length.name_length != f_namelen ||
             strncasecmp(dir_entry->file_name, f_name, f_namelen));

    return cur_addr;
}


int main(int argc, char* argv[])
{
    char     input[80];
    char*    p_name;
    unsigned dir_addr;
    unsigned header_addr;

    if (argc > 1) {
        p_name = argv[1];
    } else {
        printf("Apple GEOS Convert\nPathname:");
        p_name = gets(input);
    }

    dir_addr = get_dir_entry(p_name);

    if (dio_read(dhandle, dir_entry->key_pointer, &index_block)) {
        err_exit("dio_read.4");
    }

    header_addr = index_block.content.addr_lo[0] |
                  index_block.content.addr_hi[0] << 8;

    if (dio_read(dhandle, header_addr, &header_block)) {
        err_exit("dio_read.5");
    }

    dir_entry->storage_length  = header_block.content.dir_entry.storage_length;
    memcpy(dir_entry->file_name, header_block.content.dir_entry.file_name, 15);
    dir_entry->file_type       = header_block.content.dir_entry.file_type;
    memcpy(dir_entry->size,      header_block.content.dir_entry.size, 3);
    dir_entry->creation        = header_block.content.dir_entry.creation;
    dir_entry->version         = header_block.content.dir_entry.version;
    dir_entry->min_version     = header_block.content.dir_entry.min_version;
    dir_entry->aux_type        = header_addr;
    dir_entry->last_mod        = header_block.content.dir_entry.last_mod;

    memmove(&index_block.content.addr_lo[0],
            &index_block.content.addr_lo[1], sizeof(index_block.content.addr_lo) - 1);
    memmove(&index_block.content.addr_hi[0],
            &index_block.content.addr_hi[1], sizeof(index_block.content.addr_hi) - 1);

    index_block.content.size_lo[1] = dir_entry->size[0];
    index_block.content.size_hi[1] = dir_entry->size[1];
    index_block.content.size_lo[0] = dir_entry->size[2];
    index_block.content.size_hi[0] = 0;

    if (dio_write(dhandle, dir_addr, &dir_block)) {
        err_exit("dio_write.1");
    }

    if (dio_write(dhandle, dir_entry->key_pointer, &index_block)) {
        err_exit("dio_write.2");
    }

    if (dio_close(dhandle)) {
        err_exit("dio_close");
    }

    return EXIT_SUCCESS;
}