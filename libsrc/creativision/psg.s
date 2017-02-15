; void __fastcall__ psg_outb( unsigned char b );
; void __fastcall__ psg_delayms( unsigned char c);
; void __fastcall__ bios_playsound( void *b, unsigned char c);
; void psg_silence( void );

        .export          _psg_outb, _psg_silence, _psg_delay
        .export          _bios_playsound
        .import          popa
        .include        "creativision.inc"

_psg_outb:

        ;* Let BIOS output the value
        jmp $FE77

_psg_silence:

        jmp $FE54


_psg_delay:

        tay
l1:     lda #200
l2:     sbc #1
        bne l2

        lda #200
l3:     sbc #1
        bne l3

        dey
        bne l1

        rts


;* Creativision Sound Player
;*
;* Based on BIOS sound player.
;* Pass a pointer to a set of note triples, terminated with a tempo byte
;* and the len (max 255)

_bios_playsound:

        pha                     ; Save Length Byte
        sei

        lda         #$D5        ; BIOS volume table low
        sta         $4
        lda         #$FC        ; BIOS volume table high
        sta         $5

        jsr         popa        ; Get Sound table pointer low
        sta         $0
        jsr         popa        ; Get Sound table pointer high
        sta         $1

        pla
        tay                     ; Put length in Y
        dey
        php
        jmp         $FBED       ; Let BIOS do it's thing
