;
; Oliver Schmidt, 04.01.2005
;
; unsigned char get_ostype (void)
;

        .constructor    initostype, 9
        .export         _get_ostype, ostype

; Identify machine according to:
; Apple II Miscellaneous TechNote #7, Apple II Family Identification

        .segment        "ONCE"

initostype:
        sec
        jsr     $FE1F
        bcs     nogs
        tya
        ora     #$80
done:   sta     ostype
        rts
nogs:   ldx     #$FF
next:   inx
        lda     value,x
        ldy     index,x
        beq     done            ; $00 is no valid index
        cmp     $FB00,y
        beq     next
:       inx
        ldy     index,x
        bne     :-
        beq     next            ; Branch always

index:  .byte   $B3, $00           ; Apple ][
        .byte   $B3, $1E, $00      ; Apple ][+
        .byte   $B3, $1E, $00      ; Apple /// (emulation)
        .byte   $B3, $C0, $00      ; Apple //e
        .byte   $B3, $C0, $DD, $00 ; Apple //e Option Card
        .byte   $B3, $C0, $00      ; Apple //e (enhanced)
        .byte   $B3, $C0, $BF, $00 ; Apple //c
        .byte   $B3, $C0, $BF, $00 ; Apple //c (3.5 ROM)
        .byte   $B3, $C0, $BF, $00 ; Apple //c (Mem. Exp.)
        .byte   $B3, $C0, $BF, $00 ; Apple //c (Rev. Mem. Exp.)
        .byte   $B3, $C0, $BF, $00 ; Apple //c Plus
        .byte   $00

value:  .byte   $38, $10           ; Apple ][
        .byte   $EA, $AD, $11      ; Apple ][+
        .byte   $EA, $8A, $20      ; Apple /// (emulation)
        .byte   $06, $EA, $30      ; Apple //e
        .byte   $06, $E0, $02, $32 ; Apple //e Option Card
        .byte   $06, $E0, $31      ; Apple //e (enhanced)
        .byte   $06, $00, $FF, $40 ; Apple //c
        .byte   $06, $00, $00, $41 ; Apple //c (3.5 ROM)
        .byte   $06, $00, $03, $43 ; Apple //c (Mem. Exp.)
        .byte   $06, $00, $04, $44 ; Apple //c (Rev. Mem. Exp.)
        .byte   $06, $00, $05, $45 ; Apple //c Plus
        .byte   $00

        .code

_get_ostype:
        lda     ostype
        ldx     #$00
        rts

        .segment        "INIT"

ostype: .res    1
