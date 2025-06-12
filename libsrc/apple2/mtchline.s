;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
;
; void __fastcall__ mt_chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ mt_chline (unsigned char length);
;

.ifdef  __APPLE2ENH__

        .export         _mt_chlinexy, _mt_chline, chlinedirect
        .import         gotoxy, cputdirect

        .include        "zeropage.inc"
        .include        "apple2.inc"

_mt_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_mt_chline:
        ldx     #'_' | $80      ; Underscore, screen code

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

.endif
