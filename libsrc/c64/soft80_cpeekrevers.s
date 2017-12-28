
        .import         soft80_cpeekchar

        .export         soft80_cpeekrevers

soft80_cpeekrevers:
        jsr     soft80_cpeekchar
        txa
        ldx     #0
        rts
