/*
 * o65 file format checker
 *
 * This tool tries to verify that an o65 file conforms to the
 * o65 file spec as given at https://github.com/fachat/o65/blob/main/fileformat.adoc
 * in version 1.3.1 as of Mar 31 2025.
 *
 * Definitions and public API for inter-module communication
 *
 * Written 2026 by Spiro Trikaliotis
 *
 */

#ifndef O65CHECK_H
#define O65CHECK_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct file_s file_t;

typedef
enum {
    O65_6502_GENERIC     = 0x00,
    O65_6502_65C02       = 0x10,
    O65_6502_65SC02      = 0x20,
    O65_6502_65CE02      = 0x30,
    O65_6502_NMOS        = 0x40,
    O65_6502_EMULATION   = 0x50,
    O65_6502_RESERVED_6  = 0x60,
    O65_6502_RESERVED_7  = 0x70,
    O65_6502_RESERVED_8  = 0x80,
    O65_6502_RESERVED_9  = 0x90,
    O65_6502_RESERVED_A  = 0xA0,
    O65_6502_RESERVED_B  = 0xB0,
    O65_6502_RESERVED_C  = 0xC0,
    O65_6502_RESERVED_D  = 0xD0,
    O65_6502_RESERVED_E  = 0xE0,
    O65_6502_RESERVED_F  = 0xF0,

    O65_65816_NATIVE     = 0x00,
    O65_65816_RESERVED_1 = 0x10,
    O65_65816_RESERVED_2 = 0x20,
    O65_65816_RESERVED_3 = 0x30,
    O65_65816_RESERVED_4 = 0x40,
    O65_65816_RESERVED_5 = 0x50,
    O65_65816_RESERVED_6 = 0x60,
    O65_65816_RESERVED_7 = 0x70,
    O65_65816_RESERVED_8 = 0x80,
    O65_65816_RESERVED_9 = 0x90,
    O65_65816_RESERVED_A = 0xA0,
    O65_65816_RESERVED_B = 0xB0,
    O65_65816_RESERVED_C = 0xC0,
    O65_65816_RESERVED_D = 0xD0,
    O65_65816_RESERVED_E = 0xE0,
    O65_65816_RESERVED_F = 0xF0,


} o65_cpu2_e;

typedef
enum {
    O65_ALIGN_BYTE  = 0x00,
    O65_ALIGN_WORD  = 0x01,
    O65_ALIGN_DWORD = 0x02,
    O65_ALIGN_PAGE  = 0x03
} o65_align_e;

typedef
struct o65_option_s {
    struct o65_option_s * next;

    uint8_t               len;
    uint8_t               type;
    uint8_t               data[];
} o65_option_t;

typedef
enum {
    O65_OS__FIRST  = 1,
    O65_OS_OSA     = 1,
    O65_OS_LUNIX   = 2,
    O65_OS_CC65    = 3,
    O65_OS_OPENCBM = 4,
    O65_OS__LAST   = 4

} o65_os;

typedef struct {
    const char * name;
    uint8_t      segmentid;
    size_t       value;
} o65_global_t;

typedef
struct o65_relocation_s {
    struct o65_relocation_s * next;
    size_t                    base;
    size_t                    typebyte;
    size_t                    segmentid;
    int                       has_index;
    size_t                    index;
    int                       has_low_offset;
    int                       is_low_offset_16bit;
    size_t                    low_offset;

    size_t                    raw_data_size;
    uint8_t *                 raw_data;
} o65_relocation_t;

typedef
struct o65_param_s {
    int                cpu_is_65816;
    int                reloc_pagewise;
    int                size_is_dword;
    int                is_obj;
    int                is_simple;
    int                is_chained;
    int                bsszero;
    o65_cpu2_e         cpu2;
    o65_align_e        align;

    size_t             tbase;
    size_t             tlen;
    size_t             dbase;
    size_t             dlen;
    size_t             bbase;
    size_t             blen;
    size_t             zbase;
    size_t             zlen;
    size_t             stack;

    o65_option_t *     options;

    uint8_t *          text;
    uint8_t *          data;

    o65_relocation_t * text_relocation;
    o65_relocation_t * data_relocation;

    size_t             externals_count;
    const char **      externals;

    size_t             globals_count;
    o65_global_t *     globals;

} o65_param_t;

extern o65_param_t o65_param;

enum {
    O65_OPTION_FILENAME    = 0,
    O65_OPTION_OS          = 1,
    O65_OPTION_ASSEMBLER   = 2,
    O65_OPTION_AUTHOR      = 3,
    O65_OPTION_CREATE_DATE = 4
};


void * my_alloc(size_t member, size_t size);

file_t * file_open(const char * const filename);
void file_close(file_t * file);

size_t file_read_n_byte(file_t * file, uint8_t * data, size_t count);
void * file_get_pointer_to_current(file_t * file);

int o65_check_reference(file_t * o65_file, const char * filename);

#endif

