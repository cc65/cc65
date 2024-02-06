;
; 2002-11-15, Ullrich von Bassewitz
; 2019-11-08, Greg King
;
; Variables used for CBM file I/O
;

        .export         curunit
        .constructor    initcurunit, 30
        .destructor     updatedevnum, 30

        .include        "cx16.inc"


.segment "INIT"

curunit:
        .res    1


.segment "ONCE"

.proc   initcurunit
        lda     DEVNUM
        bne     L0
        lda     #8              ; Default is SD card
        sta     DEVNUM
L0:     sta     curunit
        rts
.endproc


.code

.proc   updatedevnum
        lda     curunit
        sta     DEVNUM
        rts
.endproc
