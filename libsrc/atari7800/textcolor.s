;
; Karri Kaksonen, 2022-04-16
;
;

        .export         _textcolor
        .export         txtcolor

        .include        "atari7800.inc"

        .data
;-----------------------------------------------------------------------------
; Holder of the text colour offset
; 0 = red, 42 = green, 84 = white
;
txtcolor:
        .byte   0

        .code

;-----------------------------------------------------------------------------
; Change the text colour
;
; Logical colour names are
; 0 = red
; 1 = green
; 2 = white
;
; The routine will also return the previous textcolor
;
        .proc   _textcolor

        beq     @L2
        sec
        sbc     #1
        beq     @L1
        lda     #84
        jmp     @L2
@L1:    lda     #42
@L2:    ldy     txtcolor
        sta     txtcolor        ; Store new textcolor
        ldx     #0
        tya
        bne     @L3
        rts                     ; Old colour was 0
@L3:    sec
        sbc     #42
        bne     @L4
        lda     #1
        rts                     ; Old colour was 1
@L4:    lda #2
        rts                     ; Old colour was 2
        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
