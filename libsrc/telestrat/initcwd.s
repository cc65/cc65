;
; Jede (jede@oric.org) 24.09.2017
;

        .export         initcwd
        .import         __cwd

        .include        "zeropage.inc"
        .include        "telestrat.inc"


initcwd:
       
        ldx     #PWD_PTR
        BRK_TELEMON XVARS
        
        sta     ptr1
        sty     ptr1+1
        
        ldy     #$00
        
loop:        
        lda     (ptr1),y
        beq     done
        sta     __cwd,y
        iny
        bne     loop
       
done:   
        sta     __cwd,y
        rts
