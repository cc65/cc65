;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtext (const char* s);
; /* Output text at the current graphics cursor position. */


        .include        "tgi-kernel.inc"

        .import         popax, negax
        .importzp       ptr3

.proc   _tgi_outtext

        sta     ptr3
        stx     ptr3+1          ; Pass s in ptr3 to driver
        pha
        txa
        pha                     ; Save s on stack for later

        jsr     tgi_curtoxy     ; Copy curx/cury into ptr1/ptr2
        jsr     tgi_outtext     ; Call the driver

        pla
        tax
        pla                     ; Restore s
        jsr     _tgi_textwidth  ; Get width of text string
        ldy     _tgi_textdir    ; Horizontal or vertical text?
        beq     @L1             ; Jump if horizontal

; Move graphics cursor for vertical text

        jsr     negax
        ldy     #2              ; Point to _tgi_cury

; Move graphics cursor for horizontal text

@L1:    clc
        adc     _tgi_curx,y
        sta     _tgi_curx,y
        txa
        adc     _tgi_curx+1,y
        sta     _tgi_curx+1,y
        rts

.endproc

