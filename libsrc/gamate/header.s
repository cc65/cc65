        ; The following symbol is used by linker config to force the module
        ; to get included into the output file
        .export         __STARTUP__: absolute = 1

        .import         Start, IRQStub, NMIStub


        .segment "STARTUP"

        .word 0                                         ; +00 checksum from 7000-7fff (simple 8bit adds)
        .byte 1, 0, 1                                   ; +02 flags
        .byte "COPYRIGHT BIT CORPORATION", 0, $ff       ; +05 copyright
        ; system vectors
        jmp     Start                                   ; +20 reset entry
        jmp     NMIStub                                 ; +23 nmi entry
        jmp     IRQStub                                 ; +26 irq entry (135 hz)
