;
; Ullrich von Bassewitz, 2003-12-20
;
; Additional zero page locations for the CBM610.
; NOTE: This file is actually linked to an application with its full contents,
; so the program comes up with the values given in this file.
;

; ------------------------------------------------------------------------

        .include        "extzp.inc"

.segment        "EXTZP" : zeropage

; The following values get initialized from a table in the startup code.
; While this sounds crazy, it has reasons that have to do with modules (and
; we have the space anyway). So when changing anything, be sure to adjust the
; initializer table
sysp1:          .word   $0000
sysp3:          .word   $0000
crtc:  	       	.word  	$0000
sid:   	       	.word  	$0000
ipccia:	       	.word  	$0000
cia:   	       	.word  	$0000
acia:  	       	.word  	$0000
tpi1:  	       	.word  	$0000
tpi2:  	       	.word  	$0000
ktab1: 	       	.word  	$0000
ktab2: 	       	.word  	$0000
ktab3: 	       	.word  	$0000
ktab4: 	       	.word  	$0000

sysp0:          .word   $0000
time:	      	.dword 	$0000
segsave:        .byte   0
scanbuf:        .byte   0
ktmp:           .byte   0
CURS_X:         .byte   0
CURS_Y:         .byte   0
CharPtr:        .word   0




