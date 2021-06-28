;
; 2016-02-28, Groepaz
; 2017-06-21, Greg King
;
; char cpeekc (void);
;

        .export         _cpeekc

        .include        "atari.inc"


_cpeekc:
        lda     OLDCHR          ; get char under cursor
        and     #<~$80          ; remove reverse bit

        ;; convert internal screen code to AtSCII

        tay
        and     #%01100000
        asl     a
        asl     a
        rol     a
        rol     a
        tax
        tya
        eor     intats,x
        ldx     #>$0000
        rts

        .rodata
intats: .byte   %00100000       ; -> %001xxxxx
        .byte   %01100000       ; -> %010xxxxx
        .byte   %01000000       ; -> %000xxxxx
        .byte   %00000000       ; -> %011xxxxx
