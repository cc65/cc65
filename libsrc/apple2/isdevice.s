;
; Oliver Schmidt, 2012-10-15
;

        .export         isdevice

        .include        "zeropage.inc"
        .include        "mli.inc"

isdevice:
        ldy     DEVCNT
:       lda     DEVLST,y
        sta     tmp1
        cpx     tmp1
        beq     :+
        dey
        bpl     :-
:       rts
