
        .export GETIN

.scope  KERNAL
        .include "cbm_kernal.inc"
.endscope

        .include "plus4.inc"

.segment "LOWCODE" ; Stay out of ROM area.

.proc   GETIN
        sta ENABLE_ROM
        jsr KERNAL::GETIN
        sta ENABLE_RAM
        rts
.endproc
