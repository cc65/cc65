        .export         _close
		.import 		addysp,popax
		.importzp 	sp,tmp2,tmp3,tmp1
	
	; int open (const char* name, int flags, ...);    /* May take a mode argument */
		.include        "telemon30.inc"
		.include 		"errno.inc"
		.include        "fcntl.inc"	
		
.proc _close
; Throw away any additional parameters passed through the ellipsis

		BRK_TELEMON XCLOSE	; launch primitive ROM
		rts
.endproc
		
		