.export GETIN

.include "plus4.inc"

KERNAL_GETIN := $FFE4

.segment "LOWCODE" ; Stay out of ROM area.

.proc   GETIN
        sta ENABLE_ROM
        jsr KERNAL_GETIN
        sta ENABLE_RAM
        rts
.endproc
