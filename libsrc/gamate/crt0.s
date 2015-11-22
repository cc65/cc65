
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import reset, irq, nmi ; FIXME


        .segment "STARTUP"

checksum:
        .word 0
        .byte 1, 0, 1
        .byte "COPYRIGHT BIT CORPORATION", 0, $ff
        jmp     reset
        jmp     nmi
        jmp     irq

