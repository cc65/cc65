;
; Ullrich von Bassewitz, 08.08.1998
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;
        .include "atari.inc"
        
        .export         _cvlinexy, _cvline
        .import         popa, _gotoxy, putchar, setcursor
        .importzp       tmp1

.ifdef __ATARI5200__
CHRCODE =       1               ; exclamation mark
.else
CHRCODE =       $7C             ; Vertical bar
.endif

_cvlinexy:
        pha                     ; Save the length
        jsr     popa            ; Get y
        jsr     _gotoxy         ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     COLCRS
        pha
        lda     #CHRCODE        ; Vertical bar
        jsr     putchar         ; Write, no cursor advance
        pla
        sta     COLCRS
        inc     ROWCRS
        dec     tmp1
        bne     L1
L9:     jmp     setcursor
