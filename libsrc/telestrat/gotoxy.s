;
; 2017-02-25, jede <jede@oric.org>
; 2017-06-15, Greg King
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy

        .import         popa
        .importzp       sp

        .include        "telestrat.inc"

gotoxy: jsr     popa            ; Get Y

.proc _gotoxy

; This function moves only the display cursor; it does not move the prompt position.
; In telemon, there is a position for the prompt, and another for the cursor.
        sta     SCRY
        jsr     popa
        sta     SCRX
        
        lda     #<SCREEN
        sta     ADSCRL

        lda     #>SCREEN
        sta     ADSCRL+1

        ldy     SCRY
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

        rts
.endproc
