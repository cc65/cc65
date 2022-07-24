;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by value in y
;

        .export         incaxy, incax4
        .importzp       tmp1
        .macpack        generic

incax4: ldy     #4
incaxy: sty     tmp1
        add     tmp1
        bcc     @L9
        inx
@L9:    rts

