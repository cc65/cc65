;
; Oliver Schmidt, 2016-06-05
;
; unsigned char doesclrscrafterexit (void);
;

        .export         _doesclrscrafterexit
        .import         done

        .include        "apple2.inc"

_doesclrscrafterexit:
        ; If the page we jump to when done equals the page
        ; of the warmstart vector we'll return to BASIC so
        ; there's no implicit clrscr() after exit().
        lda     done+2
        sec
        sbc     #>DOSWARM

        ldx     #>$0000
        rts
