;
; 2017-02-25, jede <jede@oric.org>
; 2017-06-15, Greg King
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy, update_adscr

        .import         popa, CHARCOLOR_CHANGE, BGCOLOR_CHANGE

        .include        "telestrat.inc"

gotoxy: jsr     popa            ; Get Y

.proc _gotoxy

; This function moves only the display cursor; it does not move the prompt position.
; In telemon, there is a position for the prompt, and another for the cursor.
        sta     SCRY
        jsr     popa
        sta     SCRX
        
        jsr     update_adscr          ; Update adress video ram position when SCRY et SCRX are modified
        ;       Force to put again attribute when it moves on the screen
        lda     #$01
        sta     CHARCOLOR_CHANGE
        sta     BGCOLOR_CHANGE
        rts
.endproc


.proc update_adscr
        lda     #<SCREEN
        sta     ADSCRL

        lda     #>SCREEN
        sta     ADSCRH

        ldy     SCRY
        beq     out
loop:
        lda     ADSCRL          
        clc
        adc     #$28
        bcc     skip
        inc     ADSCRH
skip:
        sta     ADSCRL        
        dey
        bne     loop
out:        
        rts
.endproc
