;
; 2020-05-02, Greg King
;
; unsigned char __fastcall__ vera_layer_enable (unsigned char layers);
; /* Display the layers that are "named" by the bit flags in layers.
; ** A value of 0b01 shows layer 0, a value of 0b10 shows layer 1,
; ** a value of 0b11 shows both layers.  Return the previous value.
; */
;

        .export         _vera_layer_enable

        .include        "cx16.inc"


mask    =       VERA::DISP::ENABLE::LAYER1 | VERA::DISP::ENABLE::LAYER0

.proc   _vera_layer_enable
        stz     VERA::CTRL              ; Use display register bank 0
        asl     a
        asl     a                       ; Shift new flags into position
        asl     a
        asl     a
        ldy     VERA::DISP::VIDEO

        eor     VERA::DISP::VIDEO
        and     #mask
        eor     VERA::DISP::VIDEO       ; Replace old flags with new flags
        sta     VERA::DISP::VIDEO

        tya
        and     #mask                   ; Get old flags
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        ldx     #>$0000
        rts
.endproc
