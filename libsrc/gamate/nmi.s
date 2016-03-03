;
; NMI handling (Gamate version)
;
        .export NMIStub

        .segment        "INIT"

NMIStub:
        ; A is saved by the BIOS
        ;pha
        ;txa
        ;pha
        ;tya
        ;pha

        ;pla
        ;tay
        ;pla
        ;tax
        ;pla
        rts
