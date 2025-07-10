;
; Ullrich von Bassewitz, 08.08.1998
; Colin Leroy-Mira, 26.05.2025
;
; void __fastcall__ dyn_chlinexy (unsigned char c, unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ dyn_chline (unsigned char c, unsigned char length);
;

.ifndef __APPLE2ENH__

        .export         _dyn_chlinexy, _dyn_chline, chlinedirect
        .import         gotoxy, cputdirect, popa
        .import         machinetype

        .include        "zeropage.inc"
        .include        "apple2.inc"

_dyn_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_dyn_chline:
        pha
        jsr     popa            ; Get the character to draw
        eor     #$80            ; Invert high bit
        tax
        pla

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
