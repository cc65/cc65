;
; Oliver Schmidt, 10.03.2004
;
; void __fastcall__ textframexy (unsigned char x, unsigned char y,
;                                unsigned char width, unsigned char height,
;                                unsigned char style);
; void __fastcall__ textframe (unsigned char width, unsigned char height,
;                              unsigned char style);
;
        .ifdef  __APPLE2ENH__

        .export         _textframexy, _textframe
        .import         popa, pusha, _gotoxy
        .import         chlinedirect, cvlinedirect

        .include        "zeropage.inc"
        .include        "apple2.inc"

WIDTH   = tmp2
HEIGHT  = tmp3
XORIGIN = tmp4
YORIGIN = ptr1

_textframexy:
        sec
        bra     :+

_textframe:
        clc
:       ldx     INVFLG
        phx                     ; Save character display mode
        ldx     #$FF
        stx     INVFLG          ; Set normal character display mode
        pha                     ; Save index
        jsr     popa            ; Get height
        sta     HEIGHT
        jsr     popa            ; Get width
        sta     WIDTH
        lda     CH
        ldx     CV
        bcc     noxy
        jsr     popa            ; Get y
        tax
        jsr     popa            ; Get x
noxy:   sta     XORIGIN
        stx     YORIGIN
        plx                     ; Restore index
loop:   lda     XOFFS,x
        clc
        bpl     :+              ; Relative to left edge?
        adc     WIDTH
:       adc     XORIGIN
        jsr     pusha
        lda     YOFFS,x
        clc
        bpl     :+              ; Relative to top?
        adc     HEIGHT
:       adc     YORIGIN
        jsr     _gotoxy         ; Call this one, will pop params
        txa
        tay
        lsr                     ; Get bit 0 (vline) into carry
        lda     LENGTH,x
        phx                     ; Save index
        ldx     CHAR,y
        bcc     hline
        clc
        adc     HEIGHT
        jsr     cvlinedirect
        bra     next
hline:  adc     WIDTH
        jsr     chlinedirect
next:   plx                     ; Restore index
        inx
        txa
        and     #$03            ; Mask style
        bne     loop
        pla
        sta     INVFLG          ; Restore character display mode
        rts

        .rodata

; 2 styles with 4 lines each make up 8 entries per table
; - even entry numbers mean horizontal lines
; - odd entry numbers mean vertical lines

; x offset for the line starting point
; - a positive value means relative to the frame left edge
; - a negative value menas relative to the frame right edge
XOFFS:  .byte   0, 0, 0, <-2, 1, 0, 1, <-2

; y offset for the line starting point
; - a positive value means relative to the frame top
; - a negative value menas relative to the frame bottom
YOFFS:  .byte   0, 1, <-2, 1, 0, 0, <-2, 0

; length of the line relative to the frame size
; - a negative value for hlines means shorter than the width
; - a negative value for vlines menas shorter than the height
LENGTH: .byte   0, <-2, 0, <-2, <-2, 0, <-2, 0

; character to use for drawing the line
; - hibit set means normal printable character
; - hibit clear means MouseText character
CHAR:   .byte   '_'|$80, '_', 'L', 'Z', 'L', 'Z', '_'|$80, '_'

        .endif                  ; __APPLE2ENH__
