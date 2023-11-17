; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: Zlib
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

.constructor initmainargs, 24
.import __argc, __argv

.segment "ONCE"

.proc initmainargs

    rts

.endproc
