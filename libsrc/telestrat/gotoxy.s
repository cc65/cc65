;
; 2017-02-25, jede <jede@oric.org>
; 2017-06-15, Greg King
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy, update_adscr

        .import         popa, OLD_CHARCOLOR, OLD_BGCOLOR

        .include        "telestrat.inc"

gotoxy: jsr     popa            ; Get Y

.proc _gotoxy

; This function moves only the display cursor; it does not move the prompt position.
; In telemon, there is a position for the prompt, and another for the cursor.

    sta     SCRY
    

    jsr     popa
    sta     SCRX

; Update adress video ram position when SCRY is modified (update_adscr)
; Fall through
.endproc

.proc update_adscr

    lda     #<SCREEN
    sta     ADSCR

    lda     #>SCREEN
    sta     ADSCR+1

    ldy     SCRY
    beq     out
loop:
    lda     ADSCR
    clc
    adc     #SCREEN_XSIZE
    bcc     skip
    inc     ADSCR+1
skip:
    sta     ADSCR
    dey
    bne     loop
out:        
    rts
.endproc
