;
; Ullrich von Bassewitz, 08.08.1998
;
; void __fastcall__ cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ cvline (unsigned char length);
;

.ifdef __APPLE2ENH__

        .export         _mt_cvlinexy, _mt_cvline
        .import         gotoxy, putchar, newline

        .include        "zeropage.inc"

_mt_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_mt_cvline:
        ldx     #$5F            ; Left vertical line MouseText character
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
