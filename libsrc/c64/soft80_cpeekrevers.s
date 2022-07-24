;
; 2017-12-28, Groepaz
;
; unsigned char cpeekrevers (void);
;

        .import         soft80_cpeekchar

        .export         soft80_cpeekrevers

soft80_cpeekrevers:
        jsr     soft80_cpeekchar
        txa
        ldx     #0
        rts
