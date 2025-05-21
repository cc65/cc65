;
; Oliver Schmidt, 2024-08-06
;

        .export         uppercasemask

        .ifndef __APPLE2ENH__
        .import         machinetype
        .constructor    detectlowercase
        .endif

        .ifndef __APPLE2ENH__

        .segment "ONCE"

detectlowercase:
        bit     machinetype
        bpl     :+

        lda     #$FF
        sta     uppercasemask
:       rts

        .endif

        .data

uppercasemask:  .byte   $DF     ; Convert to uppercase
