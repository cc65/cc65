;
; Ullrich von Bassewitz, 2003-03-07
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


