;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
; Konstantin Fedorov, 12.06.2025
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline
        .import         gotoxy, putchardirect, newline

        .include        "zeropage.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp2
:       lda     #$5C            ; vertical line character
        jsr     putchardirect   ; Write, no cursor advance
        jsr     newline         ; Advance cursor to next line
        dec     tmp2
        bne     :-
done:   rts
