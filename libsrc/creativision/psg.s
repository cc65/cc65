; void __fastcall__ psg_outb (unsigned char b);
; void __fastcall__ psg_delay (unsigned char b);
; void psg_silence (void);


        .export         _psg_outb, _psg_silence, _psg_delay

        .include        "creativision.inc"


;* Let BIOS output the value.
_psg_outb       := BIOS_POKE_PSG


_psg_silence    := BIOS_QUIET_PSG


_psg_delay:
        tay
l1:     lda     #200
l2:     sbc     #1
        bne     l2

        lda     #200
l3:     sbc     #1
        bne     l3

        dey
        bne     l1
        rts
