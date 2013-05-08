#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <device.h>
#include <dio.h>

unsigned char info_signature[3] = {3, 21, 63 | 0x80};

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
} index_block, master_block, vlir_block;

union {
    unsigned char bytes[512];
    struct {
        unsigned           reserved;
        unsigned char      info_block[254];
        unsigned char      vlir_records[128];
        struct dir_entry_t dir_entry;
    } content;
} header_block;


static void err_exit(char *operation, unsigned char oserr)
{
    if (oserr) {
        fprintf(stderr, "%s - err:%02x - %s",
                operation, (int)_oserror, _stroserror(_oserror));
    } else {
        fprintf(stderr, "%s",
                operation);
    }
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
        err_exit("opendir", 1);
    }
    dirent = readdir(dir);
    if (!dirent) {
        err_exit("readdir", 1);
    }

    /* Field header_pointer directly follows field last_mod */
    cur_addr = *(unsigned*)(&dirent->d_mtime.hour + 1);

    dhandle = dio_open(getcurrentdevice());
    if (!dhandle) {
        err_exit("dio_open", 1);
    }

    if (dio_read(dhandle, cur_addr, &dir_block)) {
        err_exit("dio_read.1", 1);
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
                    err_exit("dio_read.2", 1);
                }

                /* Read next directory block */
                if (dio_read(dhandle, cur_addr, &dir_block)) {
                    err_exit("dio_read.3", 1);
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
    char          input[80];
    char*         p_name;
    unsigned      dir_addr;
    unsigned      header_addr;
    unsigned char index;
    unsigned long size;

    if (argc > 1) {
        p_name = argv[1];
    } else {
        printf("\n"
               "Apple GEOS Convert 1.0\n"
               "----------------------\n"
               "\n"
               "Pathname:");
        p_name = gets(input);
    }

    dir_addr = get_dir_entry(p_name);

    /* Read index block */
    if (dio_read(dhandle, dir_entry->key_pointer, &index_block)) {
        err_exit("dio_read.4", 1);
    }

    /* First pointer is header block */
    header_addr = index_block.content.addr_lo[0] |
                  index_block.content.addr_hi[0] << 8;

    /* Read header block */
    if (dio_read(dhandle, header_addr, &header_block)) {
        err_exit("dio_read.5", 1);
    }

    /* Do some sanity check */
    for (index = 0; index < sizeof(info_signature); ++index) {
        if (header_block.content.info_block[index] != info_signature[index]) {
            err_exit("file signature mismatch", 0);
        }
    }

    /* Check ProDOS storage type in directory entry template */
    if (header_block.content.dir_entry.storage_length.storage_type == 2)
    {

        /* ProDOS sapling file means GEOS Sequential file*/
        printf("\nSequential file\n");

        /* Remove header block pointer from pointer list */
        memmove(&index_block.content.addr_lo[0],
                &index_block.content.addr_lo[1], sizeof(index_block.content.addr_lo) - 1);
        memmove(&index_block.content.addr_hi[0],
                &index_block.content.addr_hi[1], sizeof(index_block.content.addr_hi) - 1);

        /* Get file size from ProDOS directory entry template */
        size = (unsigned long)(header_block.content.dir_entry.size[0])       |
               (unsigned long)(header_block.content.dir_entry.size[1]) <<  8 |
               (unsigned long)(header_block.content.dir_entry.size[2]) << 16;

    } else {

        /* ProDOS tree file means GEOS VLIR file */
        unsigned      vlir_addr;
        unsigned long vlir_size;
        unsigned char vlir_blocks;
        unsigned char record = 0;

        printf("\nVLIR file\n");

        /* Skip header block pointer */
        index = 1;
        size  = 0;

        while (1) {

            /* Get next VLIR index pointer from index block */
            vlir_addr = index_block.content.addr_lo[index] |
                        index_block.content.addr_hi[index] << 8;
            ++index;

            /* Check for end of pointer list */
            if (vlir_addr == 0) {
                break;
            }

            /* Check for empty VLIRs */
            while (header_block.content.vlir_records[record] == 0xFF) {

                /* Add empty VLIR index pointer to to master index block */
                master_block.content.addr_lo[record] = 0xFF;
                master_block.content.addr_hi[record] = 0xFF;
                ++record;
            }

            /* Add VLIR index pointer to master index block */
            master_block.content.addr_lo[record] = (unsigned char)(vlir_addr     );
            master_block.content.addr_hi[record] = (unsigned char)(vlir_addr >> 8);
            ++record;

            /* Read VLIR index block */
            if (dio_read(dhandle, vlir_addr, &vlir_block)) {
                err_exit("dio_read.6", 1);
            }

            /* Get VLIR size from VLIR index block */
            vlir_size = (unsigned long)(vlir_block.content.size_lo[1])       |
                        (unsigned long)(vlir_block.content.size_hi[1]) <<  8 |
                        (unsigned long)(vlir_block.content.size_lo[0]) << 16 |
                        (unsigned long)(vlir_block.content.size_hi[0]) << 24;

            printf("VLIR %u size %lu bytes\n", record - 1, vlir_size);

            /* Compute VLIR block size */
            vlir_blocks = (unsigned char)((vlir_size + 511) / 512);

            /* Copy VLIR block pointers from index block to VLIR index block */
            memcpy(&vlir_block.content.addr_lo[0],
                   &index_block.content.addr_lo[index], vlir_blocks);
            memcpy(&vlir_block.content.addr_hi[0],
                   &index_block.content.addr_hi[index], vlir_blocks);
            index += vlir_blocks;

            /* Write back VLIR index block */
            if (dio_write(dhandle, vlir_addr, &vlir_block)) {
                err_exit("dio_write.1", 1);
            }

            /* Add VLIR size to file size */
            size += vlir_size;
        }

        /* Replace (by now completely read) index block with
           (by now completely created) master index block */
        index_block = master_block;
    }

    printf("File size %lu bytes\n\n", size);

    /* Set file size in index block */
    index_block.content.size_lo[1] = (unsigned char)(size      );
    index_block.content.size_hi[1] = (unsigned char)(size >>  8);
    index_block.content.size_lo[0] = (unsigned char)(size >> 16);
    index_block.content.size_hi[0] = (unsigned char)(size >> 24);

    /* Write index block */
    if (dio_write(dhandle, dir_entry->key_pointer, &index_block)) {
        err_exit("dio_write.2", 1);
    }

    /* Copy selected fields from directory entry template to directory block */
    dir_entry->storage_length  = header_block.content.dir_entry.storage_length;
    memcpy(dir_entry->file_name, header_block.content.dir_entry.file_name, 15);
    dir_entry->file_type       = header_block.content.dir_entry.file_type;
    dir_entry->size[0]         = (unsigned char)(size      );
    dir_entry->size[1]         = (unsigned char)(size >>  8);
    dir_entry->size[2]         = (unsigned char)(size >> 16);
    dir_entry->creation        = header_block.content.dir_entry.creation;
    dir_entry->version         = header_block.content.dir_entry.version;
    dir_entry->min_version     = header_block.content.dir_entry.min_version;
    dir_entry->aux_type        = header_addr;
    dir_entry->last_mod        = header_block.content.dir_entry.last_mod;

    /* Write directory block */
    if (dio_write(dhandle, dir_addr, &dir_block)) {
        err_exit("dio_write.3", 1);
    }

    /* We're done */
    if (dio_close(dhandle)) {
        err_exit("dio_close", 1);
    }

    printf("Convert to '%.*s' successful", dir_entry->storage_length.name_length,
                                           dir_entry->file_name);
    getchar();
    return EXIT_SUCCESS;
}
