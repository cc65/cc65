; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: Zlib
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

.constructor initenv, 24
.import __environ, __envcount, __envsize

.segment "ONCE"

.proc initenv

    rts

.endproc
