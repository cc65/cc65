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
; Force to set again color if cursor moves       
; $FF is used because we know that it's impossible to have this value with a color
; It prevents a bug : If bgcolor or textcolor is set to black for example with no char displays,
; next cputsxy will not set the attribute if y coordinate changes
    lda     #$FF                  
    sta     OLD_CHARCOLOR         
    sta     OLD_BGCOLOR           

    lda     #<SCREEN
    sta     ADSCRL

    lda     #>SCREEN
    sta     ADSCRH

    ldy     SCRY
    beq     out
loop:
    lda     ADSCRL          
    clc
    adc     #SCREEN_XSIZE
    bcc     skip
    inc     ADSCRH
skip:
    sta     ADSCRL        
    dey
    bne     loop
out:        
    rts
.endproc
