/*
 * Copyright (c) 2023 Rumbledethumps
 *
 * SPDX-License-Identifier: Zlib
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-License-Identifier: Unlicense
 */

#include <rp6502.h>

int __fastcall__ phi2(void)
{
    return ria_call_int(RIA_OP_PHI2);
}
