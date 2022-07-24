;
; Christian Groessler, 02-Apr-2019
;
; Screen size variables
;

        .export         screensize
        .importzp       screen_width, screen_height
        .include        "atari.inc"

.proc   screensize

        ldx     #screen_width
        ldy     #screen_height
        rts

.endproc


