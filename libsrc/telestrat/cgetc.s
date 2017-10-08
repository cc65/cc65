;
; jede jede@oric.org 2017-10-01
;
    .export    _cgetc
   
    .import    cursor

    
    .include   "telestrat.inc"

.proc _cgetc
        ; this routine could be quicker if we wrote in page 2 variables, but it's better to use telemon routine in that case, because telemon can manage 4 I/O
        lda     cursor      ; if cursor equal to 0, then switch off cursor
        beq     switchoff_cursor
       
        ldx     #$00         ; x is the first screen
        BRK_TELEMON(XCSSCR)  ; display cursor
        jmp     loop         ; could be replaced by a bne/beq but 'jmp' is cleaner than a bne/beq which could expect some
        
switchoff_cursor:        
        ldx     #$00         ; x is the first screen
        BRK_TELEMON(XCOSCR)  ; switch off cursor        
        
loop:   
        BRK_TELEMON XRD0
        bcs     loop
        rts
.endproc	
