;
; Mark Keates, Christian Groessler, Piotr Fusik, Karri Kaksonen
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

        .export         _cputc
        .import         gotox, gotoy, pusha0
        .import         pushax
        .import         _screen
        .import         txtcolor

        .include        "atari7800.inc"
        .include        "extzp.inc"

        .code

;---------------------------------------------------------------------------
; 8x16 routine

umula0:
        ldy     #8                 ; Number of bits
        lda     #0
        lsr     ptr7800            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptrtmp
        tax
        lda     ptrtmp+1           ; hi byte of left op
        clc
        adc     ptr7800+1
        sta     ptr7800+1
        txa

@L1:    ror     ptr7800+1
        ror     a
        ror     ptr7800
        dey
        bne     @L0
        tax
        lda     ptr7800            ; Load the result
        rts

;-----------------------------------------------------------------------------
; Put a character on screen
;
; The code will handle newlines that wrap to start of screen
;
        .proc   _cputc

        cmp     #$0A            ; LF
        bne     @L4
@L1:    lda     CURS_Y          ; newline
        cmp     #(screenrows-1)
        bne     @L2
        lda     #0
        beq     @L3
@L2:    clc
        adc     #1
@L3:    jsr     gotoy
        lda     #0
        jmp     gotox

@L4:
        cmp     #$20            ; ' '
        bne     @L5
        lda     #$00
        jmp     @L10
@L5:
        cmp     #$3F            ; '?'
        bne     @L6
        lda     #$02
        jmp     @L9
@L6:
        cmp     #$7C            ; '|'
        bne     @L7
        lda     #$06
        jmp     @L9
@L7:
        cmp     #$41            ; >= 'A'
        bcc     @L8
        and     #$5F            ; make upper case
        sec
        sbc     #($41 - 17)
        jmp     @L9
@L8:
        sec                     ; >= '*'
        sbc     #($2A - 1)
@L9:
        clc
        adc     txtcolor
@L10:
        asl
        pha

        lda     #0
        sta     ptr7800+1
        sta     ptrtmp+1
        lda     CURS_Y          ; Find position on screen buffer
        sta     ptr7800
        lda     #charsperline
        sta     ptrtmp
        jsr     umula0
        clc
        adc     CURS_X
        bcc     @L11
        inx
@L11:   clc
        adc     #<(_screen)
        sta     ptr7800
        bcc     @L12
        inx
@L12:   txa
        clc
        adc     #>(_screen)
        sta     ptr7800+1

        pla                     ; Print character on screen
        ldy     #0
        sta     (ptr7800),y

        lda     CURS_X          ; Increment cursor
        cmp     #(charsperline-1)
        beq     @L1
        clc
        adc     #1
        jmp     gotox

        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
