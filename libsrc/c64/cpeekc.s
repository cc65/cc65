;
; Ullrich von Bassewitz, 1998-08-06, 2009-09-26
;
; unsigned char cpeekc (void);
;

        .export         _cpeekc

        .include        "c64.inc"
        
_cpeekc:
        ldy     CURS_X
    	lda     (SCREEN_PTR),y
    	and     #127    ; ignore revers bit, because it is intended for cpeekrev
    	cmp     #32     ; something like iscntrl(c)
    	bcs     :+      ; no control, so continue with other range tests
    	ora     #64     ; mk pet 0-31 -> asc 64-95
    	bcc     return  
:	
    	cmp     #64     ; pet asc for range 32-63 already matching
    	bcc     return
    	cmp     #96     
    	bcc     add32
    	adc     #31     ; add 32 (sec!) and fall through second add (eff +64) 
add32:
    	adc     #32     ; assume clc, here (either from bcc or from prev adc)
return:
    	rts

