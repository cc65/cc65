;
; Christian Groessler, June 2004
;
; void __fastcall__ _scroll (signed char numlines);
; numlines > 0  scrolls up
; numlines < 0  scrolls down
;

        .include        "atari.inc"
        .importzp       tmp1,tmp4,ptr1,ptr2
        .import         mul40,_clrscr
        .export         __scroll

.proc   __scroll

        cmp     #0
        beq     jmpfin
;       cmp     #$80
;       bcc     up
        bpl     up

;scroll down
        eor     #$ff
        clc
        adc     #1              ; make positive
        sta     tmp1

        cmp     #24             ; scroll >= the whole screen?
        bcc     down_ok
        jmp     _clrscr

down_ok:lda     SAVMSC
        clc
        adc     #<(40*23)
        sta     ptr1
        sta     ptr2
        lda     SAVMSC+1
        adc     #>(40*23)
        sta     ptr1+1          ; point to last line on screen
        sta     ptr2+1

        lda     tmp1
        jsr     mul40
        sta     tmp4
        lda     ptr2
        sec
        sbc     tmp4
        sta     ptr2
        stx     tmp4
        lda     ptr2+1
        sbc     tmp4
        sta     ptr2+1

        lda     #24             ; # of lines on screen
        sec
        sbc     tmp1            ; # of lines to move
        tax

        ;very simple, could be improved

scrold: ldy     #39             ; # of chars on a line - 1
copy_d: lda     (ptr2),y
        sta     (ptr1),y
        dey
        bpl     copy_d
        lda     ptr1
        sec
        sbc     #40
        sta     ptr1
        bcs     u1
        dec     ptr1+1
u1:     lda     ptr2
        sec
        sbc     #40
        sta     ptr2
        bcs     u2
        dec     ptr2+1
u2:     dex
        bne     scrold

        ; fill new scrolled in lines with space

        ldx     tmp1            ; # of new lines
fild:   lda     #0
        ldy     #39
fill_d: sta     (ptr1),y
        dey
        bpl     fill_d
        dex
jmpfin: beq     finish
        lda     ptr1
        sec
        sbc     #40
        sta     ptr1
        bcs     u3
        dec     ptr1+1
u3:     jmp     fild

;scroll up
up:     sta     tmp1            ; # of lines to scroll
        cmp     #24             ; scroll >= the whole screen?
        bcc     up_ok
        jmp     _clrscr

        ;multiply by 40 (xsize)
up_ok:  jsr     mul40
        clc
        adc     SAVMSC          ; add start of screen mem
        sta     ptr2
        txa
        adc     SAVMSC+1
        sta     ptr2+1
        lda     SAVMSC+1
        sta     ptr1+1
        lda     SAVMSC
        sta     ptr1
        lda     #24             ; # of lines on screen
        sec
        sbc     tmp1            ; # of lines to move
        tax

        ;very simple, could be improved

scroll: ldy     #39             ; # of chars on a line - 1
copy_l: lda     (ptr2),y
        sta     (ptr1),y
        dey
        bpl     copy_l
        lda     #40
        clc
        adc     ptr1
        sta     ptr1
        bcc     l1
        inc     ptr1+1
l1:     lda     #40
        clc
        adc     ptr2
        sta     ptr2
        bcc     l2
        inc     ptr2+1
l2:     dex
        bne     scroll

        ; fill new scrolled in lines with space

        ldx     tmp1            ; # of new lines
fill:   lda     #0
        ldy     #39
fill_l: sta     (ptr1),y
        dey
        bpl     fill_l
        dex
        beq     finish
        lda     #40
        clc
        adc     ptr1
        sta     ptr1
        bcc     l3
        inc     ptr1+1
l3:     jmp     fill

finish: rts

.endproc
