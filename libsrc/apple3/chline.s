;
; Robert Justice
;
; void __fastcall__ chlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ chline (unsigned char c, unsigned char length);
;

        .export         _chlinexy, _chline, chlinedirect
        .import         gotoxy, _cputc, popa

        .include        "zeropage.inc"

_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_chline:
        ldx     #'-'            ; Underscore, screen code

chlinedirect:
        stx     tmp1
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp2
:       lda     tmp1            ; Screen code
        jsr     _cputc          ; output
        dec     tmp2
        bne     :-
done:   rts
