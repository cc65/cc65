;
; Ullrich von Bassewitz, 2003-02-16
;
; Additional zero page locations for the CBM510.
; NOTE: The zeropage locations contained in this file get initialized
; in the startup code, so if you change anything here, be sure to check
; not only the linker config, but also the startup file.
;

; ------------------------------------------------------------------------

        .include        "extzp.inc"

.segment        "EXTZP" : zeropage

vic:  		.res    2
sid:		.res    2
cia1:		.res    2
cia2:  		.res    2
acia:  		.res    2
tpi1:		.res    2
tpi2:  		.res    2
ktab1:		.res    2
ktab2:	       	.res    2
ktab3:		.res    2
ktab4:		.res    2
time:		.res    4

