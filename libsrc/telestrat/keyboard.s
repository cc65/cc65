; jede jede@oric.org 2017-01-22

    .export         _key
				
    .include        "zeropage.inc"		
    .include        "telestrat.inc"

; char key(void);
		
.proc _key
    BRK_TELEMON XRDW0 ; read keyboard
    rts
.endproc
	
