;
; 2020-05-02, Greg King
;
; unsigned char __fastcall__ vera_sprites_enable (unsigned char mode);
; /* Enable the sprite engine when mode is non-zero (true);
; ** disable sprites when mode is zero.  Return the previous mode.
; */
;

        .export         _vera_sprites_enable

        .include        "cx16.inc"


.proc   _vera_sprites_enable
        stz     VERA::CTRL                      ; Use display register bank 0
        tax                                     ; Is mode true?
        beq     :+
        lda     #VERA::DISP::ENABLE::SPRITES    ; Yes
:       ldy     VERA::DISP::VIDEO

        eor     VERA::DISP::VIDEO
        and     #VERA::DISP::ENABLE::SPRITES
        eor     VERA::DISP::VIDEO               ; Replace old flag with new flag
        sta     VERA::DISP::VIDEO

        tya
        and     #VERA::DISP::ENABLE::SPRITES    ; Get old value
        asl     a
        asl     a
        rol     a                               ; Make it boolean
        ldx     #>$0000
        rts
.endproc
