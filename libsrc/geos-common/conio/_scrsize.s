;
; Maciej 'YTM/Elysium' Witkowiak
;
; Screen size variables
;
; 6.3.2001, 17.4.2003


        .export xsize, ysize
        .export screensize
        .importzp cursor_r, cursor_c
        .import _cursor
        .constructor initscrsize
                      
        .include "geossym.inc"

.segment        "INIT"

initscrsize:
.ifdef __GEOS_CBM__
        lda graphMode
        bpl L1
        lda #80                 ; 80 columns (more or less)
        .byte $2c
L1:     lda #40                 ; 40 columns (more or less)
        sta xsize
        lda #25                 ; something like that for Y size
.else
        lda #70                 ; 70 columns (more or less)
        sta xsize
        lda #24                 ; something like that for Y size
.endif
        sta ysize
        lda #0
        sta cursor_c
        sta cursor_r
        jmp _cursor             ; home and update cursor

.code

screensize: 
        ldx xsize
        ldy ysize
        rts

.bss

xsize:
        .res 1
ysize:
        .res 1
