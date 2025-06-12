;
; Oliver Schmidt, 2024-08-06
;

.ifndef __APPLE2ENH__

        .export         uppercasemask

        .import         machinetype
        .constructor    detectlowercase

        .segment "ONCE"

detectlowercase:
        bit     machinetype
        bpl     :+

        lda     #$FF
        sta     uppercasemask
:       rts


        .data

uppercasemask:  .byte   $DF     ; Convert to uppercase

.endif
