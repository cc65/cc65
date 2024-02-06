;
; Ullrich von Bassewitz, 08.08.1998
;
; void __fastcall__ chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ chline (unsigned char length);
;

        .export         _chlinexy, _chline, chlinedirect
        .import         gotoxy, cputdirect

        .include        "zeropage.inc"
        .include        "apple2.inc"

_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_chline:
        .ifdef  __APPLE2ENH__
        ldx     #'_' | $80      ; Underscore, screen code
        .else
        ldx     #'-' | $80      ; Minus, screen code
        .endif

chlinedirect:
        stx     tmp1
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp2
:       lda     tmp1            ; Screen code
        jsr     cputdirect      ; Direct output
        dec     tmp2
        bne     :-
done:   rts
