;
; Ullrich von Bassewitz, 08.08.1998
;
; void __fastcall__ cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline
        .import         gotoxy, putchar, newline

        .include        "zeropage.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        .ifdef  __APPLE2ENH__
        ldx     #$5F            ; Left vertical line MouseText character
        .else
        ldx     #'!' | $80      ; Exclamation mark, screen code
        .endif

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
