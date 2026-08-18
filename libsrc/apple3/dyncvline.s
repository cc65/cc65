;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
; Robert Justice, 2026
;
; void __fastcall__ dyn_cvlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ dyn_cvline (unsigned char c, unsigned char length);
;
;

        .export         _dyn_cvlinexy, _dyn_cvline
        .import         gotoxy, _cputc, _gotoy, _gotox, popa
        .import         getpos, cursory, cursorx, wrconss

        .include        "zeropage.inc"
        .include        "apple3.inc"
        .include        "sos.inc"

_dyn_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_dyn_cvline:
        pha
        jsr     popa            ; Get the character to draw
        sta     cvlinbuf        ; And update write buffer
        pla
        sta     tmp2            ; count
        beq     done            ; Jump if done

        jsr     getpos
        lda     cursory         ; Set Y pos        
        sta     cvlinbuf+2
        lda     cursorx         ; Set X pos
        sta     cvlinbuf+4

        lda     #5
        ldx     #0
        sta     sosparam + SOS::RW::REQUEST_COUNT
        stx     sosparam + SOS::RW::REQUEST_COUNT+1

        lda     #<cvlinbuf
        ldx     #>cvlinbuf
        sta     sosparam + SOS::RW::DATA_BUFFER
        stx     sosparam + SOS::RW::DATA_BUFFER+1

:       inc     cvlinbuf+2      ; Update Y pos
        jsr     wrconss         ; Output
        dec     tmp2
        bne     :-

done:   rts


        .data

cvlinbuf:
        .byte   0
        .byte   25      ; Vert pos
        .byte   0
        .byte   24      ; Horiz pos
        .byte   0
