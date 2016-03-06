;
; Ullrich von Bassewitz, 15.11.2002
;
; Variables used for CBM file I/O
;

        .export         curunit
        .constructor    initcurunit, 30
        .importzp       devnum


.segment "INIT"

curunit:
        .res    1


.segment "ONCE"

.proc   initcurunit

        lda     devnum
        bne     @L0
        lda     #8              ; Default is disk
        sta     devnum
@L0:    sta     curunit
        rts

.endproc
