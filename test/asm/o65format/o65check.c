/*
 * o65 file format checker
 *
 * This tool tries to verify that an o65 file conforms to the
 * o65 file spec as given at https://github.com/fachat/o65/blob/main/fileformat.adoc
 * in version 1.3.1 as of Mar 31 2025.
 *
 * Calling:
 *   o65check [o65file] [o65filedescription]
 *
 * Written 2026 by Spiro Trikaliotis
 *
 */

#include "o65check.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define PRGNAME "o65check"

// #define VERBOSE 1
// #define VERBOSE_BYTE_LEVEL 1

#if VERBOSE
  #define VERBOSE_FPRINTF(...) \
      fprintf(__VA_ARGS__)
#else
  #define VERBOSE_FPRINTF(...) do { } while (0)
#endif

o65_param_t o65_param = { 0 };

static
char * o65_os_string[] = {
    "OS/A header supplement",
    "Lunix header supplement",
    "CC65 generic module",
    "OpenCBM floppy module"
};

static o65_option_t * o65_option_alloc(uint8_t type, size_t size)
{
    o65_option_t * p = NULL;

    p = my_alloc(1, sizeof *p + size);

    p->len  = size;
    p->type = type;

    p->next = o65_param.options;
    o65_param.options = p;

    return p;
}

#define CHECK_NO_ERROR(_var) \
    do { \
        if (_var < 0) { \
            fprintf(stderr, PRGNAME ": Error while reading " #_var "\n"); \
            return _var; \
        } \
    } while (0)

#define CHECK(_var, _condition, _text) \
    do { \
        CHECK_NO_ERROR(_var); \
        if (!(_condition)) { \
            fprintf(stderr, PRGNAME ": " _text " --- " #_condition ", has value %zu = 0x%04zX\n", (size_t) _var, (size_t) _var); \
            return 11; \
        } \
    } \
    while (0)


static size_t file_read_1_byte(file_t * file)
{
    unsigned char byte = 0;

    size_t status = file_read_n_byte(file, &byte, 1);
    CHECK_NO_ERROR(status);

    return byte;
}

static size_t file_read_2_byte(file_t * file)
{
    size_t byte1 = file_read_1_byte(file);
    if (byte1 < 0) return byte1;

    size_t byte2 = file_read_1_byte(file);
    if (byte2 < 0) return byte2;

    return byte1 | (byte2 << 8);
}

static size_t file_read_4_byte(file_t * file)
{
    size_t word1 = file_read_2_byte(file);
    if (word1 < 0) return word1;

    size_t word2 = file_read_2_byte(file);
    if (word2 < 0) return word2;

    return word1 | (word2 << 16);
}

static size_t file_read_word(file_t * file)
{
    return file_read_2_byte(file);
}

static size_t file_read_dword(file_t * file)
{
    return file_read_4_byte(file);
}

static size_t file_read_size(file_t * file)
{
    if (o65_param.size_is_dword) {
        return file_read_4_byte(file);
    }
    else {
        return file_read_2_byte(file);
    }
}

static size_t file_read_string(file_t * file, const char ** string)
{
    const int max_len = 0x10000;
    int i;

    char * s = my_alloc(1, max_len);

    *string = s;

    for (i = 0; i < max_len; ++i) {
        size_t ch = file_read_1_byte(file);
        CHECK_NO_ERROR(ch);

        s[i] = ch;

        if (ch == 0) break;
    }

    if (i >= max_len) {
        fprintf(stderr, "String exceeds maximum size of 0x%08X.", max_len);
        return 5;
    }

    return 0;
}

enum {
    O65_MODE_BITMASK_CPU     = 0x8000u,
    O65_MODE_BITMASK_RELOC   = 0x4000u,
    O65_MODE_BITMASK_SIZE    = 0x2000u,
    O65_MODE_BITMASK_OBJ     = 0x1000u,
    O65_MODE_BITMASK_SIMPLE  = 0x0800u,
    O65_MODE_BITMASK_CHAIN   = 0x0400u,
    O65_MODE_BITMASK_BSSZERO = 0x0200u,
    O65_MODE_BITMASK_CPU2    = 0x00F0u,
    O65_MODE_BITMASK_ALIGN   = 0x0003u,
    O65_MODE_BITMASK_UNUSED  = 0x010Cu,
};

static int o65file_header_read_mode(file_t * o65_file)
{
    size_t mode = file_read_word(o65_file);
    CHECK_NO_ERROR(mode);

    o65_param.cpu_is_65816   = mode & O65_MODE_BITMASK_CPU;
    o65_param.reloc_pagewise = mode & O65_MODE_BITMASK_RELOC;
    o65_param.size_is_dword  = mode & O65_MODE_BITMASK_SIZE;
    o65_param.is_obj         = mode & O65_MODE_BITMASK_OBJ;
    o65_param.is_simple      = mode & O65_MODE_BITMASK_SIMPLE;
    o65_param.is_chained     = mode & O65_MODE_BITMASK_CHAIN;
    o65_param.bsszero        = mode & O65_MODE_BITMASK_BSSZERO;
    o65_param.cpu2           = mode & O65_MODE_BITMASK_CPU2;
    o65_param.align          = mode & O65_MODE_BITMASK_ALIGN;

    CHECK(mode, (mode & O65_MODE_BITMASK_UNUSED) == 0, "undefined mode params are set");

    if (o65_param.reloc_pagewise) {
        CHECK(o65_param.align, o65_param.align == O65_ALIGN_PAGE, "is reloc=1, align should be 3 (pagewise)");
    }

    return 0;
}

static int o65file_header_read_segment_data(file_t * o65_file)
{
    o65_param.tbase = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.tbase);
    o65_param.tlen = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.tlen);

    o65_param.dbase = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.dbase);
    o65_param.dlen = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.dlen);

    o65_param.bbase = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.bbase);
    o65_param.blen = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.blen);

    o65_param.zbase = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.zbase);
    o65_param.zlen = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.zlen);

    o65_param.stack = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.stack);

    if (o65_param.is_simple) {
        /* check if the condition for a simple file is fulfilled */
        CHECK(o65_param.dbase, o65_param.dbase == o65_param.tbase + o65_param.tlen, "simple format: data segment is directly after text segment");
        CHECK(o65_param.bbase, o65_param.bbase == o65_param.dbase + o65_param.dlen, "simple format: data segment is directly after text segment");
    }

    return 0;
}

static const char * get_o65_os_version_string(o65_option_t * option)
{
    int os = option->data[0];

    static char s[200];

    if (os < O65_OS__FIRST || os > O65_OS__LAST) {
        sprintf(s, "unknown os 0x%02X", os);
    }
    else {
        strcpy(s, o65_os_string[os - O65_OS__FIRST]);
    }

    return s;
}

static const char * get_o65_reloc_typebyte_string(uint8_t typebyte) {
    switch (typebyte) {
        case 0x00: return "(0x00)";
        case 0x20: return "LOW   ";
        case 0x40: return "HIGH  ";
        case 0x60: return "(0x60)";
        case 0x80: return "WORD  ";
        case 0xA0: return "SEG   ";
        case 0xC0: return "SEGADR";
        case 0xE0: return "(0xE0)";

        default:   return "------";
    }
}

static const char * get_o65_reloc_segmentid_string(uint8_t segmentid) {
    switch (segmentid) {
        case  0: return "undef";
        case  1: return "(abs)";
        case  2: return "text ";
        case  3: return "data ";
        case  4: return "bss  ";
        case  5: return "zero ";

        default: return "-----";
    }
}

static int o65file_header_read_options(file_t * o65_file)
{
    size_t option_len = 1;
    size_t option_type;

    while (option_len > 0) {
        option_len = file_read_1_byte(o65_file);
        CHECK_NO_ERROR(option_len);

        if (option_len == 0) break;

        option_type = file_read_1_byte(o65_file);
        CHECK_NO_ERROR(option_type);

        o65_option_t * option = o65_option_alloc(option_type, option_len);

        if (option_len < 2) {
            fprintf(stderr, "Got option 0x%02zX with length %zu (0x%04zX), this is not allowed\n", option_type, option_len, option_len);
            exit(4);
        }

        int status = file_read_n_byte(o65_file, option->data, option_len - 2);
        CHECK_NO_ERROR(status);

#if VERBOSE
        switch (option->type) {
            case O65_OPTION_FILENAME:    fprintf(stderr, "Filename:      %s\n", option->data); break;
            case O65_OPTION_OS:          fprintf(stderr, "OS:            %s (0x%02zX)\n", get_o65_os_version_string(option), option->data[0]); break;
            case O65_OPTION_ASSEMBLER:   fprintf(stderr, "Assembler:     %s\n", option->data); break;
            case O65_OPTION_AUTHOR:      fprintf(stderr, "Author:        %s\n", option->data); break;
            case O65_OPTION_CREATE_DATE: fprintf(stderr, "Creation date: %s\n", option->data); break;
            default:                     fprintf(stderr, "Unknown option 0x%02zX", option->type); break;
        }
#endif // VERBOSE
    }

    return 0;
}


static int o65file_read_header(file_t * o65_file)
{

    size_t marker1;
    size_t marker2;
    size_t magic1;
    size_t magic2;
    size_t magic3;
    size_t version;

    int    status;

    marker1 = file_read_1_byte(o65_file);
    CHECK(marker1, marker1 == 0x01, "1st byte of marker is wrong");

    marker2 = file_read_1_byte(o65_file);
    CHECK(marker2, marker2 == 0x00, "2nd byte of marker is wrong");

    magic1 = file_read_1_byte(o65_file);
    CHECK(magic1, magic1 == 0x6f, "1st byte of magic is wrong");

    magic2 = file_read_1_byte(o65_file);
    CHECK(magic2, magic2 == 0x36, "2st byte of magic is wrong");

    magic3 = file_read_1_byte(o65_file);
    CHECK(magic3, magic3 == 0x35, "3st byte of magic is wrong");

    version = file_read_1_byte(o65_file);
    CHECK(version, version == 0, "version is wrong");

    if (0 != (status = o65file_header_read_mode(o65_file))) return status;
    if (0 != (status = o65file_header_read_segment_data(o65_file))) return status;
    if (0 != (status = o65file_header_read_options(o65_file))) return status;

    return status;
}

static int o65file_read_segment(file_t * o65_file, uint8_t ** data, size_t length)
{
    uint8_t * p = my_alloc(1, length);

    *data = p;

    int status = file_read_n_byte(o65_file, p, length);
    CHECK_NO_ERROR(status);

    return 0;
}

static int o65file_read_segment_text(file_t * o65_file)
{
    return o65file_read_segment(o65_file, &o65_param.text, o65_param.tlen);
}

static int o65file_read_segment_data(file_t * o65_file)
{
    return o65file_read_segment(o65_file, &o65_param.data, o65_param.dlen);
}

static int o65file_read_externals(file_t * o65_file)
{
    int i;

    o65_param.externals_count = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.externals_count);

    CHECK(o65_param.externals_count, o65_param.externals_count < 0x10000, "Although not forbidden, these are too many externals for practical purposes");

    o65_param.externals = my_alloc(o65_param.externals_count, sizeof(char *));

    VERBOSE_FPRINTF(stderr, "\nExternals:\n");
    for (i = 0; i < o65_param.externals_count; ++i) {
        int status = file_read_string(o65_file, &o65_param.externals[i]);
        CHECK_NO_ERROR(status);

    VERBOSE_FPRINTF(stderr, "- %s\n", o65_param.externals[i]);
    }

    return 0;
}

static int o65file_read_relocation(file_t * o65_file, size_t base, o65_relocation_t ** next_relocation_entry)
{
    --base;

    while (1) {
        size_t ch;

        o65_relocation_t * relocation_entry = my_alloc(1, sizeof * relocation_entry);

        relocation_entry->raw_data = file_get_pointer_to_current(o65_file);

        do {
            ch = file_read_1_byte(o65_file);
            CHECK_NO_ERROR(ch);
            ++relocation_entry->raw_data_size;

            if (ch == 0xFFu) {
                base = base + 0xFEu;
            }
        } while (ch == 0xFFu);

        if (ch == 0x00) break;

        base = base + ch;

        relocation_entry->base = base;

        ch = file_read_1_byte(o65_file);
        CHECK_NO_ERROR(ch);
        ++relocation_entry->raw_data_size;

        relocation_entry->typebyte = ch & 0xE0;
        relocation_entry->segmentid = ch & 0x1F;

        if (relocation_entry->segmentid == 0x00) {
            // when undefined, we have an undefined index. Read it
            relocation_entry->has_index = 1;
            relocation_entry->index = file_read_size(o65_file);
            CHECK_NO_ERROR(relocation_entry->index);
            relocation_entry->raw_data_size += o65_param.size_is_dword ? 4 : 2;
        }

        switch (relocation_entry->typebyte) {
            case 0x40: // HIGH
                relocation_entry->has_low_offset = 1;
                relocation_entry->low_offset = file_read_1_byte(o65_file);
                CHECK_NO_ERROR(relocation_entry->low_offset);
                ++relocation_entry->raw_data_size;
                break;

            case 0xA0: // SEG
                relocation_entry->has_low_offset = 1;
                relocation_entry->is_low_offset_16bit = 1;
                relocation_entry->low_offset = file_read_2_byte(o65_file);
                CHECK_NO_ERROR(relocation_entry->low_offset);
                relocation_entry->raw_data_size += 2;
                break;

            default:
                break;
        }

        relocation_entry->raw_data_size = (uint8_t *) file_get_pointer_to_current(o65_file) - relocation_entry->raw_data;

        *next_relocation_entry = relocation_entry;
        next_relocation_entry = &relocation_entry->next;

#if VERBOSE
        {
            char index_text[80] = "";
            char low_offset_text[80] = "";

            if (relocation_entry->has_index) {
                snprintf(index_text, sizeof index_text - 1, ", index = %zd", relocation_entry->index);
                index_text[sizeof index_text - 1] = 0;
            }

            if (relocation_entry->has_low_offset) {
                snprintf(low_offset_text, sizeof low_offset_text - 1, ", low_offset = 0x%0*zX", relocation_entry->is_low_offset_16bit ? 4 : 2, relocation_entry->low_offset);
                low_offset_text[sizeof low_offset_text - 1] = 0;
            }

            VERBOSE_FPRINTF(stderr, "- reloc: count = %u, base 0x%04zX, type = %s (0x%02zX), segment = %s (0x%02zX)%s%s\n",
                    relocation_entry->raw_data_size,
                    relocation_entry->base,
                    get_o65_reloc_typebyte_string(relocation_entry->typebyte), relocation_entry->typebyte,
                    get_o65_reloc_segmentid_string(relocation_entry->segmentid), relocation_entry->segmentid,
                    index_text,
                    low_offset_text);

#if VERBOSE_BYTE_LEVEL
            for (int i = 0; i < relocation_entry->raw_data_size; ++i) {
                VERBOSE_FPRINTF(stderr, "  $%02X, ", relocation_entry->raw_data[i]);
            }
            VERBOSE_FPRINTF(stderr, "\n");
#endif
        }
#endif

    }

    return 0;
}

static int o65file_read_relocation_text(file_t * o65_file)
{
    VERBOSE_FPRINTF(stderr, "Read text relocation:\n");

    return o65file_read_relocation(o65_file, o65_param.tbase, &o65_param.text_relocation);
}

static int o65file_read_relocation_data(file_t * o65_file)
{
    VERBOSE_FPRINTF(stderr, "Read data relocation:\n");

    return o65file_read_relocation(o65_file, o65_param.dbase, &o65_param.data_relocation);
}

static int o65file_read_globals(file_t * o65_file)
{
    int i;

    o65_param.globals_count = file_read_size(o65_file);
    CHECK_NO_ERROR(o65_param.globals_count);

    CHECK(o65_param.globals_count, o65_param.globals_count < 0x10000, "Although not forbidden, these are too many globals for practical purposes");

    o65_param.globals = my_alloc(o65_param.globals_count, sizeof(o65_global_t));

    VERBOSE_FPRINTF(stderr, "\nGlobals:\n");

    for (i = 0; i < o65_param.globals_count; ++i) {
        int status = file_read_string(o65_file, &o65_param.globals[i].name);
        CHECK_NO_ERROR(status);

        o65_param.globals[i].segmentid = file_read_1_byte(o65_file);
        CHECK_NO_ERROR(o65_param.globals[i].segmentid);
        o65_param.globals[i].value     = file_read_size(o65_file);
        CHECK_NO_ERROR(o65_param.globals[i].value);

        VERBOSE_FPRINTF(stderr,
                "- %-20s, segmentid = 0x%02X, value = 0x%0*zX\n",
                o65_param.globals[i].name,
                o65_param.globals[i].segmentid,
                o65_param.size_is_dword ? 8 : 4,
                o65_param.globals[i].value);
    }

    return 0;
}

static int check_o65file(const char * const o65_filename, const char * const reference_filename)
{
    file_t * o65_file = NULL;

    int status = 10;

    fprintf(stderr, "Checking file %s, comparing with reference %s\n", o65_filename, reference_filename);

    do {
        o65_file = file_open(o65_filename);
        if (!o65_file) break;

        ++status;

        status = 0;

        do {
            if (0 != (status = o65file_read_header(o65_file))) break;
            if (0 != (status = o65file_read_segment_text(o65_file))) break;
            if (0 != (status = o65file_read_segment_data(o65_file))) break;
            if (0 != (status = o65file_read_externals(o65_file))) break;
            if (0 != (status = o65file_read_relocation_text(o65_file))) break;
            if (0 != (status = o65file_read_relocation_data(o65_file))) break;
            if (0 != (status = o65file_read_globals(o65_file))) break;

            if (0 != (status = o65_check_reference(o65_file, reference_filename))) break;
        } while (o65_param.is_chained);

    } while (0);

    file_close(o65_file);

    return status;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, PRGNAME ": Please give the o65 file to check!\n");
        return 1;
    }

    if (argc > 3) {
        fprintf(stderr, PRGNAME ": Too many parameter given\n");
        return 2;
    }

    return check_o65file(argv[1], argv[2]);
}
