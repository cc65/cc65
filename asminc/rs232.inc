;
; rs232.inc
;
; (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that makes the constants and structures from rs232.h
; available for asm code.



; Error codes returned by all functions
RS_ERR_OK      	       	       	= $00  	; Not an error - relax
RS_ERR_NOT_INITIALIZED 	       	= $01   ; Module not initialized
RS_ERR_BAUD_TOO_FAST   	       	= $02   ; Cannot handle baud rate
RS_ERR_BAUD_NOT_AVAIL  	       	= $03   ; Baud rate not available
RS_ERR_NO_DATA 	       	       	= $04   ; Nothing to read
RS_ERR_OVERFLOW        	       	= $05   ; No room in send buffer
RS_ERR_INIT_FAILED     	       	= $06   ; Initialization of RS232 routines failed
