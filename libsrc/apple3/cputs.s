;
; Ullrich von Bassewitz, 06.08.1998
; Robert Justice, 2026
;
; void cputsxy (unsigned char x, unsigned char y, const char* s);
; void cputs (const char* s);
;
;
; Output in one SOS call to improve console speed
;

        .export         _cputsxy, _cputs
        .import         gotoxy, wrconss
        .importzp       ptr1

        .include        "sos.inc"

_cputsxy:
        sta     ptr1            ; Save s for later
        stx     ptr1+1
        jsr     gotoxy          ; Set cursor, pop x and y
        lda     ptr1
        ldx     ptr1+1

_cputs: sta     ptr1
        stx     ptr1+1
        sta     sosparam + SOS::RW::DATA_BUFFER
        stx     sosparam + SOS::RW::DATA_BUFFER+1

        ; find length of s
        ldy     #0
        sty     sosparam + SOS::RW::REQUEST_COUNT+1
:       lda     (ptr1),y
        beq     :+
        iny
        bne     :-
        inc     sosparam + SOS::RW::REQUEST_COUNT+1
        bne     :-

:       sty     sosparam + SOS::RW::REQUEST_COUNT

        ; write it out
        jmp     wrconss

