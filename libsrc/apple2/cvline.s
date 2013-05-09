;
; Ullrich von Bassewitz, 08.08.1998
;
; void __fastcall__ cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline, cvlinedirect
        .import         popa, _gotoxy, putchar, newline

        .include        "zeropage.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr     popa            ; Get y
        jsr     _gotoxy         ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        .ifdef  __APPLE2ENH__
        ldx     #'|' | $80      ; Vertical line, screen code
        .else
        ldx     #'!' | $80      ; Vertical line, screen code
        .endif

cvlinedirect:
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp1
:       txa                     ; Screen code
        jsr     putchar         ; Write, no cursor advance
        jsr     newline         ; Advance cursor to next line
        dec     tmp1
        bne     :-
done:   rts
