;
; Oliver Schmidt, 2024-08-06
;
; unsigned char __fastcall__ allow_lowercase (unsigned char onoff);
;

        .export         _allow_lowercase
        .import         uppercasemask, return0, return1

_allow_lowercase:
        tax
        lda     values,x
        ldx     uppercasemask
        sta     uppercasemask
        cpx     #$FF
        beq     :+
        jmp     return0
:       jmp     return1

        .rodata

values: .byte   $DF         ; Force uppercase
        .byte   $FF         ; Keep lowercase
