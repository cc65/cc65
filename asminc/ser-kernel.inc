;****************************************************************************
;*                                                                          *
;*                              ser-kernel.inc                              *
;*                                                                          *
;*                        Serial communication API                          *
;*                                                                          *
;*                                                                          *
;*                                                                          *
;*(C) 2003      Ullrich von Bassewitz                                       *
;*              Römerstrasse 52                                             *
;*              D-70794 Filderstadt                                         *
;*EMail:        uz@cc65.org                                                 *
;*                                                                          *
;*                                                                          *
;*This software is provided 'as-is', without any expressed or implied       *
;*warranty.  In no event will the authors be held liable for any damages    *
;*arising from the use of this software.                                    *
;*                                                                          *
;*Permission is granted to anyone to use this software for any purpose,     *
;*including commercial applications, and to alter it and redistribute it    *
;*freely, subject to the following restrictions:                            *
;*                                                                          *
;*1. The origin of this software must not be misrepresented; you must not   *
;*   claim that you wrote the original software. If you use this software   *
;*   in a product, an acknowledgment in the product documentation would be  *
;*   appreciated but is not required.                                       *
;*2. Altered source versions must be plainly marked as such, and must not   *
;*   be misrepresented as being the original software.                      *
;*3. This notice may not be removed or altered from any source              *
;*   distribution.                                                          *
;*                                                                          *
;****************************************************************************



;------------------------------------------------------------------------------
; Offsets into the driver header

SER_HDR_ID              = 0             ; Contains 0x73, 0x65, 0x72 ("ser")
SER_HDR_VERSION         = 3             ; Interface version

SER_HDR_JUMPTAB         = 4
SER_HDR_INSTALL         = SER_HDR_JUMPTAB+0     ; INSTALL routine
SER_HDR_UNINSTALL       = SER_HDR_JUMPTAB+2     ; UNINSTALL routine
SER_HDR_PARAMS          = SER_HDR_JUMPTAB+4     ; PARAMS routine
SER_HDR_GET             = SER_HDR_JUMPTAB+6     ; GET routine
SER_HDR_PUT             = SER_HDR_JUMPTAB+8     ; PUT routine
SER_HDR_PAUSE           = SER_HDR_JUMPTAB+10    ; PAUSE routine
SER_HDR_UNPAUSE         = SER_HDR_JUMPTAB+12    ; UNPAUSE routine
SER_HDR_STATUS          = SER_HDR_JUMPTAB+14    ; STATUS routine
SER_HDR_IOCTL           = SER_HDR_JUMPTAB+16    ; IOCTL routine

SER_HDR_JUMPCOUNT       = 9                     ; Number of jump vectors

;------------------------------------------------------------------------------
; Offsets into the struct passed to ser_params

SER_PARAMS_BAUDRATE     =       0       ; Baudrate
SER_PARAMS_DATABITS     =       1       ; Number of data bits
SER_PARAMS_STOPBITS     =       2       ; Number of stop bits
SER_PARAMS_PARITY       =       3       ; Parity setting
SER_PARAMS_HANDSHAKE    =       4       ; Type of handshake to use

;------------------------------------------------------------------------------
; Serial parameters

; Baudrate
SER_BAUD_45_5           =       $00
SER_BAUD_50            	=       $01
SER_BAUD_75             =       $02
SER_BAUD_110           	=       $03
SER_BAUD_134_5         	=       $04
SER_BAUD_150            =       $05
SER_BAUD_300           	=       $06
SER_BAUD_600           	=       $07
SER_BAUD_1200          	=       $08
SER_BAUD_1800           =       $09
SER_BAUD_2400          	=       $0A
SER_BAUD_4800          	=       $0B
SER_BAUD_9600          	=       $0C
SER_BAUD_19200         	=       $0D
SER_BAUD_38400         	=       $0E
SER_BAUD_57600         	=       $0F
SER_BAUD_115200        	=       $10
SER_BAUD_230400        	=       $11

; Data bit settings
SER_BITS_5             	=       $00
SER_BITS_6             	=       $01
SER_BITS_7             	=       $02
SER_BITS_8             	=       $03

; Stop bit settings
SER_STOP_1             	=       $00
SER_STOP_2             	=       $01

; Parity
SER_PAR_NONE           	=       $00
SER_PAR_ODD            	=       $01
SER_PAR_EVEN           	=       $02
SER_PAR_MARK           	=       $03
SER_PAR_SPACE          	=       $04

; Handshake
SER_HS_NONE             =       $00    ; No handshake
SER_HS_HW               =       $01    ; Hardware (RTS/CTS) handshake
SER_HS_SW               =       $02    ; Software handshake

; Bit masks to mask out things from the status returned by rs232_status
SER_STATUS_PE          	=       $01	; Parity error
SER_STATUS_FE  	       	=       $02	; Framing error
SER_STATUS_OE           =       $04	; Overrun error
SER_STATUS_DCD 	       	=       $20	; NOT data carrier detect
SER_STATUS_DSR 	       	=       $40	; NOT data set ready

;------------------------------------------------------------------------------
; Variables

       	.global	_ser_drv                         ; Pointer to driver

;------------------------------------------------------------------------------
; Driver entry points

        .global ser_install
        .global ser_uninstall
        .global ser_params
        .global ser_get
        .global ser_put
        .global ser_pause
        .global ser_unpause
        .global ser_status
        .global ser_ioctl


;------------------------------------------------------------------------------
; ASM functions

        .global _ser_unload
        .global _ser_install
        .global _ser_uninstall
        .global _ser_params
        .global _ser_get
        .global _ser_put
        .global _ser_pause
        .global _ser_unpause
        .global _ser_status
        .global _ser_ioctl


