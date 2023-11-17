; Copyright (c) 2023 Rumbledethumps
;
; SPDX-License-Identifier: Zlib
; SPDX-License-Identifier: BSD-3-Clause
; SPDX-License-Identifier: Unlicense

.export initirq, doneirq
.import callirq, _exit

.include "rp6502.inc"

.segment "ONCE"

initirq:
    lda #<handler
    ldx #>handler
    sei
    sta $FFFE
    stx $FFFF
    cli
    rts

.code

doneirq:
    sei
    rts

.segment "LOWCODE"

handler:
    cld
    phx
    tsx
    pha
    inx
    inx
    lda $100,X
    and #$10
    bne break
    phy
    jsr callirq
    ply
    pla
    plx
    rti

break:
    lda #$FF
    sta RIA_A
    jmp _exit
