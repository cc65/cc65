; void __fastcall__ bios_playsound (void *a, unsigned char b);


        .export         _bios_playsound

        .import         popax

        .include        "creativision.inc"


songptr :=      $00             ; Points to current tune data
volptr  :=      $04             ; Points to current volume table


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

        lda     #<$FCD5         ; BIOS decreasing-volume table
        ldx     #>$FCD5
        sta     volptr
        stx     volptr+1

        jsr     popax           ; Get tune array pointer
        sta     songptr
        stx     songptr+1

        pla
        tay
        dey                     ; Point to tempo byte
        jmp     BIOS_PLAY_SONG  ; Let BIOS do its thing
