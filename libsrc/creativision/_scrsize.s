;*
;** _scrsize.s
;*

        .export screensize
    
        .include        "creativision.inc"
    
.proc   screensize

        ldx     #SCREEN_COLS
        ldy     #SCREEN_ROWS
        rts
        
.endproc
