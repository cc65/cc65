        .export         _key
		.importzp 		sp,tmp2,tmp3,tmp1
		
		.include        "telemon30.inc"


; char key(void);
		
.proc _key
	BRK_TELEMON XRDW0 ; read keyboard
	rts
.endproc
	
