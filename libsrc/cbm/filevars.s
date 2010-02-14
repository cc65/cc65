;
; Ullrich von Bassewitz, 15.11.2002
;
; Variables used for CBM file I/O
;

        .export         __filetype
        .export         __curunit
        .constructor    initcurunit, 30
        .importzp       devnum


.data

__filetype:
        .byte   'u'             ; Create user files by default


.bss

__curunit:
        .res    1


.segment "INIT"

.proc   initcurunit

        lda     devnum
        bne     @L0
        lda     #8              ; Default is disk
        sta     devnum
@L0:    sta     __curunit
        rts

.endproc
