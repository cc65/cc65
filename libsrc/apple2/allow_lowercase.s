;
; Oliver Schmidt, 2024-08-06
;
; unsigned char __fastcall__ allow_lowercase (unsigned char onoff);
;

.ifndef __APPLE2ENH__

        .export         _allow_lowercase
        .import         return0
        .import         uppercasemask, return1

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

.endif
