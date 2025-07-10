;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
;
; void __fastcall__ dyn_cvlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ dyn_cvline (unsigned char c, unsigned char length);
;

.ifndef __APPLE2ENH__

        .export         _dyn_cvlinexy, _dyn_cvline
        .import         gotoxy, putchar, newline, popa
        .import         machinetype

        .include        "zeropage.inc"

_dyn_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_dyn_cvline:
        pha
        jsr     popa            ; Get the character to draw
        eor     #$80            ; Invert high bit
        tax
        pla

        stx     tmp1
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp2
:       lda     tmp1            ; Screen code
        jsr     putchar         ; Write, no cursor advance
        jsr     newline         ; Advance cursor to next line
        dec     tmp2
        bne     :-
done:   rts

.endif
