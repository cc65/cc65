/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: Zlib
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>

int __fastcall__ stdin_opt(unsigned long ctrl_bits, unsigned char str_length)
{
    ria_push_long(ctrl_bits);
    ria_set_a(str_length);
    return ria_call_int_errno(RIA_OP_STDIN_OPT);
}
