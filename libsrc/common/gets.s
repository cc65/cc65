;
; Colin Leroy-Mira, 2024
;
; char* __fastcall__ gets (char* s)
;

        .export         _gets
        .import         _fgets, _stdin, popax, pushax
        .importzp       ptr4

_gets:
        ; Push buffer
        sta     ptr4
        stx     ptr4+1
        jsr     pushax

        ; Push size (there's no limit!)
        lda     #$FF
        tax
        jsr     pushax

        lda     _stdin
        ldx     _stdin+1

        jsr     _fgets

        ; Check return value
        bne     :+
        cpx     #$00
        bne     :+
        rts

:       ; At least one byte written.
        jsr     pushax          ; Store returned pointer

        ; Remove \n if there is one.
        lda     ptr4            ; _fgets returns with ptr4 at
        bne     :+              ; end of buffer
        dec     ptr4+1
:       dec     ptr4
        lda     (ptr4),y        ; _fgets returns with Y=0
        cmp     #$0A
        bne     :+
        tya
        sta     (ptr4),y        ; Set terminator over \n

:       jmp     popax
