;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
; Robert Justice, 2026
;
; void __fastcall__ dyn_chlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ dyn_chline (unsigned char c, unsigned char length);
;

        .export         _dyn_chlinexy, _dyn_chline, chlinedirect
        .import         gotoxy, _cputc, popa, wrconss

        .include        "zeropage.inc"
        .include        "sos.inc"

_dyn_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_dyn_chline:
        pha
        jsr     popa            ; Get the character to draw
        tax
        pla

chlinedirect:
        stx     tmp1
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done

        sta     sosparam + SOS::RW::REQUEST_COUNT
        tax     
        lda     tmp1            ; Screen code
:       sta     chlinbuf,x
        dex
        bpl     :-

        lda     #<chlinbuf
        ldx     #>chlinbuf
        sta     sosparam + SOS::RW::DATA_BUFFER
        stx     sosparam + SOS::RW::DATA_BUFFER+1

        ldx     #0
        stx     sosparam + SOS::RW::REQUEST_COUNT+1

        jsr     wrconss          ; output

done:   rts


        .bss

chlinbuf:
        .res    80
