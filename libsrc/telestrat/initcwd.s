;
; Oliver Schmidt, 18.04.2005
;

        .export         initcwd
        .import         __cwd

        .include        "zeropage.inc"
        .include        "telestrat.inc"


initcwd:
        ldx     #PWD_PTR
        BRK_TELEMON XVARS
        sta     tmp1
        sty     tmp1+1

        ldy     #$00
loop:        
        lda     (tmp1),y
        beq     done
        sta     __cwd,y
        iny
        bne     loop
       
done:   
        sta     __cwd,y
        rts
