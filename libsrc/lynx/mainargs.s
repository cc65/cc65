;
; Ullrich von Bassewitz, 2004-10-08
;
; Setup arguments for main
;


	.constructor    initmainargs, 24
       	.import         __argc, __argv


;---------------------------------------------------------------------------
; Setup arguments for main

.proc   initmainargs

        rts

.endproc


