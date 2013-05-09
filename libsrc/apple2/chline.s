;
; Ullrich von Bassewitz, 08.08.1998
;
; void __fastcall__ chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void __fastcall__ chline (unsigned char length);
;

        .export         _chlinexy, _chline, chlinedirect
        .import         popa, _gotoxy, cputdirect

        .include        "zeropage.inc"
        .include        "apple2.inc"

_chlinexy:
        pha                     ; Save the length
        jsr     popa            ; Get y
        jsr     _gotoxy         ; Call this one, will pop params
        pla                     ; Restore the length and run into _chline

_chline:
        .ifdef  __APPLE2ENH__
        ldx     #'S'            ; MouseText character
        ldy     INVFLG
        cpy     #$FF            ; Normal character display mode?
        beq     chlinedirect
        .endif
        ldx     #'-' | $80      ; Horizontal line, screen code

chlinedirect:
        cmp     #$00            ; Is the length zero?
        beq     done            ; Jump if done
        sta     tmp1
:       txa                     ; Screen code
        jsr     cputdirect      ; Direct output
        dec     tmp1
        bne     :-
done:   rts
