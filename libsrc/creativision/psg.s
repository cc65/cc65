; void __fastcall__ psg_outb (unsigned char b);
; void __fastcall__ psg_delay (unsigned char b);
; void __fastcall__ bios_playsound (void *a, unsigned char b);
; void psg_silence (void);


        .export         _psg_outb, _psg_silence, _psg_delay
        .export         _bios_playsound

        .import         popax

        .include        "creativision.inc"


songptr :=      $00             ; Points to current tune data
volptr  :=      $04             ; Points to current volume table

_psg_outb:
        ;* Let BIOS output the value.
        jmp     $FE77


_psg_silence:
        jmp     $FE54


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


;* Creativision Sound Player
;* Based on BIOS song player.
;*
;* Pass a pointer to a set of note triples, terminated with a tempo byte;
;* and pass the length of the triples and tempo (max 255).
;*
;* Note: tune data must be stored backwards.

_bios_playsound:
        php
        pha                     ; Save tune length
        sei

        lda     #<$FCD5         ; A BIOS volume table
        ldx     #>$FCD5
        sta     volptr
        stx     volptr+1

        jsr     popax           ; Get tune array pointer
        sta     songptr
        stx     songptr+1

        pla
        tay
        dey                     ; Point to tempo byte
        jmp     $FBED           ; Let BIOS do its thing
