;
; extzp.inc for the Lynx
;
; Ullrich von Bassewitz, 2004-11-06
;
; Assembler include file that imports the runtime zero page locations used
; by the Lynx runtime, ready for usage in asm code.
;


       	.global         __iodat: zp
        .global         __iodir: zp
        .global         __viddma: zp
        .global         __sprsys: zp


