; jede jede@oric.org 2017-01-22
        
    .export         _paper,_hires,_text,_circle,_curset, _switchOffCursor
	
    .include        "zeropage.inc"
    .include        "telestrat.inc"
    .import 	    popa
		
		
.proc _paper
    ldx #0 ; First window
    ; A contains the paper
    BRK_TELEMON XPAPER
    rts
.endproc
		
; XINK is bugged, it corrupt memory : removing from export		
.proc _ink
    ldx #0 ; First window
    ; A contains the ink
    BRK_TELEMON XINK
    rts
.endproc		
		
; can be optimized with a macro		
.proc _hires
    BRK_TELEMON XHIRES
    rts
.endproc

.proc _text
    BRK_TELEMON XTEXT
    rts
.endproc

.proc _curset
    jsr popa ; Pixel
    jsr popa
	sta HRSX
    jsr popa
    sta HRSY
    BRK_TELEMON XCURSE
    rts
.endproc

.proc _circle
    sta HRS1
    BRK_TELEMON XCIRCL
    rts
.endproc

.proc _switchOffCursor
    ldx #0
    BRK_TELEMON XCOSCR
    rts
.endproc


