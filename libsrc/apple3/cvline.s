;
; Robert Justice
;
; void __fastcall__ cvlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ cvline (unsigned char c, unsigned char length);
;
; needs font loaded with mousetext chars in the first 32
;

        .export         _cvlinexy, _cvline
        .import         gotoxy, _cputc, _gotoy, _gotox, popa
        .import         getpos, cursory, cursorx

        .include        "zeropage.inc"
        .include        "apple3.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        sta     tmp2
        jsr     getpos
        ldx     #'|'
        stx     tmp1
        lda     tmp2
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
:       lda     tmp1            ; Screen code
        jsr     _cputc          ; Write
        inc     cursory
        lda     cursory
        jsr     _gotoy
        lda     cursorx
        jsr     _gotox
        dec     tmp2
        bne     :-
done:   rts
