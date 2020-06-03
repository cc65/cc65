;
; Oliver Schmidt, 2012-10-15
;

        .export         isdevice

        .include        "zeropage.inc"
        .include        "mli.inc"

isdevice:
        lda     $bfff
        cmp     #$25
        lda     #$f0
        bcc     :+
        lda     #$f3
:       sta     tmp1
        ldy     DEVCNT
:       txa
        eor     DEVLST,y
        and     tmp1
        beq     :+
        dey
        bpl     :-
:       rts
